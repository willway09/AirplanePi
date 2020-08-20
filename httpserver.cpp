//C compatible headers
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <unistd.h>
#include <pthread.h>
#include <gpiod.h>

//C++ versions of C headers
#include <cstring>
#include <cctype>
#include <cmath>

//C++ headers
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>

//User-defined headers
#include "onewire.hpp"

//Structs for sharing groups of values between main() and threads
struct PwmValues {
	int pwmPercent = 0;
	int frequency = 500;
	pthread_mutex_t pwmControlMutex = PTHREAD_MUTEX_INITIALIZER;
};

struct EnvironmentValues {
	double temperature = 0;
	double humidity = 0;
	pthread_mutex_t environmentControlMutex = PTHREAD_MUTEX_INITIALIZER;
};

struct ValuesContainer {
	struct PwmValues* pwmValues;
	struct EnvironmentValues* enviroValues;
};

//Control PWM signal
void* pwmControl(void* vargp) {

	//Convert void vargp pointer to appropriate PwmValues pointer
	struct PwmValues* pwmValues = (struct PwmValues*) vargp;

	//Open GPIO chip
	struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");

	//If openining fails, exit thread
	if(!chip) {
		std::cerr << "Error opening chip" << std::endl;
		return NULL;
	}

	//Get specific PIN from chip
	struct gpiod_line* line = gpiod_chip_get_line(chip, 23); //CHANGE PWM OUTPUT PIN HERE - currently 23

	//If opening pin fails, exit thread
	if(!line) {
		gpiod_chip_close(chip);
		std::cerr << "Error opening line" << std::endl;
		return NULL;
	}


	//Create configuration struct for pin - set to output type;
	struct gpiod_line_request_config outputConfig = { "pwmControl", GPIOD_LINE_REQUEST_DIRECTION_OUTPUT, 0 };


	//Configure GPIO PIN, and if failure, exit thread
	if(gpiod_line_request(line, &outputConfig, 0) == -1) {
		gpiod_chip_close(chip);
		std::cerr << "Error requesting line configuration" << std::endl;
		return NULL;
	}

	

	while(true) { //Main PWM loop
	
		//Declare local variables to copy from pwmValues struct, which is volatile
		int localPercent, localFrequency;
		
		//Lock mutex so that pwmValues can be read
		pthread_mutex_lock(&(pwmValues->pwmControlMutex));
	
			localPercent = pwmValues->pwmPercent;
			localFrequency = pwmValues->frequency;

		//Unlock mutex
		pthread_mutex_unlock(&(pwmValues->pwmControlMutex));

		//Prevent a divide-by-zero error
		if(localFrequency == 0) localFrequency = 1;


		//Compute the delays, in microseconds, for the PWM pulse
		double periodOn = (((double)localPercent / 100 ) / localFrequency) * 1E6;
		double periodOff = (((double)(100 - localPercent) / 100 ) / localFrequency) * 1E6;

		if(localPercent == 0) { //Condition for 0% duty cycle
			gpiod_line_set_value(line, 0);
			usleep((long)(periodOff + periodOn));
		} else if(localPercent == 100) { //Condition for 100% duty cycle
			gpiod_line_set_value(line, 1);
			usleep((long)(periodOff + periodOn));
		} else { //PWM control
			gpiod_line_set_value(line, 1);
			usleep((long) periodOn);
			gpiod_line_set_value(line, 0);
			usleep((long) periodOff);;
		}
		
		//Note that usleep() both delays for an (approximately) appropriate ammount of time
		//and yields resources back to the CPU, preventing the infinite while loop
		//from spinlocking the CPU

	}
}

//Get values from X-DTHS-P temperature/humidity sensor
void* environmentControl(void* vargp) {
	
	//Convert void vargp pointer to appropriate EnvironmentValues pointer
	struct EnvironmentValues* enviroValues = (struct EnvironmentValues*) vargp;
	
	//Open GPIO chip
	struct gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
	
	//If opening fails, exit thread
	if(!chip) {
		std::cerr << "Error opening chip" << std::endl;
		return NULL;
	}
	
	//Get specific PIN from chip
	struct gpiod_line* line = gpiod_chip_get_line(chip, 24); //CHANGE ONEWIRE DATA PIN HERE - currently 24
	
	//If opening pin fails, exit thread
	if(!line) {
		gpiod_chip_close(chip);
		std::cerr << "Error opening line" << std::endl;
		return NULL;
	}
	
	
	//Configure GPIO PIN, and if failure, exit thread
	//oneWireOutputConfig defined in onewire.hpp
	if(gpiod_line_request(line, &oneWireOutputConfig, 0) == -1) {
		gpiod_chip_close(chip);
		std::cerr << "Error requesting line configuration" << std::endl;
		return NULL;
	}
	
	
	//Note that reset(), sendByte(), and readByte() are all defined and implemented in onewire.hpp
	
	while(true) { //Main sensor loop
	
		reset(line); //Send reset pulse
	
		
		sendByte(line, 0xCC); //Address all devices (only one on line)
			
				//This step appears to be necessary - sets ROS bit, which empirically seems necesary based on
				//observations of functionality of ControlByWeb products
			
				sendByte(line, 0xCC); //Write Conditional Search Register
				
				//Register 0x008D
				sendByte(line, 0x8D);
				sendByte(line, 0x00);
			
				sendByte(line, 0x04); //Write value 0x04
			
			
		reset(line); //Send reset pulse
	
		
		sendByte(line, 0xcc); //Address all devices (only one on line)
			sendByte(line, 0xF5); //Channel Access read
		
			//Read relevant values
			unsigned char deviceCode = readByte(line);
				
			unsigned char tempLsb = readByte(line);
			unsigned char tempMsb = readByte(line);
			
			unsigned char humLsb = readByte(line);
			unsigned char humMsb = readByte(line);
			
			unsigned char crc = readByte(line);
			
			double localTemp = std::round(100 * ( -45 + 175. * ((tempMsb << 8) | tempLsb) / ((1 << 16) - 1) )) / 100;
			double localHum = std::round(100 * (100. * ((humMsb << 8) | humLsb) / ((1 << 16) - 1))) / 100;
			
		
		reset(line); //Reset line
		
		//Add conditional check for CRC - only update if CRC of bytes matches CRC sent
		
		pthread_mutex_lock(&(enviroValues->environmentControlMutex));
		
			enviroValues->temperature = localTemp;
			enviroValues->humidity = localHum;
		
		pthread_mutex_unlock(&(enviroValues->environmentControlMutex));
		
		
		sleep(3); //Sensor manual recommends delays of 2 seconds between readings,
		          //or else usage will cause self-heating - 3 second delay is fine
		          //Manual: https://www.controlbyweb.com/accessories/x-dths-p-users-manual.pdf
	}
	
}


std::string mainPage() { //Return the main web page, for user interfacing

	std::ifstream indexFile("index.html"); //Open index.html, which is the main GUI webpage file
	
	std::string responseContent(""); //Declare responseContent, which is the non-header component of the response

	if(indexFile.is_open()) { //If the file opened successfully

		while(!(indexFile.eof())) { //Copy file into responseContent string
			std::string append;
			std::getline(indexFile, append);
			responseContent += append + "\n";
		}

		indexFile.close();
		
	} else { //Else send error message
		responseContent += "<b>Could not open index.html</b>";
	}
	

	//Create HTTP response
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Length: " + std::to_string(responseContent.length()) + "\r\n";
	response += "\r\n";
	response += responseContent;

	return response;

}

//Basic method for determining if string is a positive integer
bool isNumber(std::string& value) {
	for(char c : value) {
		if(!std::isdigit(c)) { //If not digit, return false
			return false;
		}
	}
	if(value.length() == 0) return false; //If string length is zero, above loop did not run; still return false
	return true; //If at end of method, return true
}

//This is the backbone of the REST-style interface
//Parameters passed to this page are handled, and it returns an XML file
//containing the state of the PWM signal
std::string statePage(std::unordered_map<std::string, std::string>& params, void* data) {

	//Convert void data pointer to appropriate PwmValues pointer
	struct PwmValues* pwmValues = ((struct ValuesContainer*) data)->pwmValues;
	
	//Convert void data pointer to appropriate EnvironmentValues pointer
	struct EnvironmentValues* enviroValues = ((struct ValuesContainer*) data)->enviroValues;

	//Declare localPercent, an integer representation of the parameter value, and set if percent is an integer
	int localPercent = -1;
	if(isNumber(params["percent"]))
		localPercent = std::stoi(params["percent"]);


	int localFrequency = -1;
	if(isNumber(params["frequency"]))
		localFrequency = std::stoi(params["frequency"]);


	//Lock mutex briefly to update values
	pthread_mutex_lock(&(pwmValues->pwmControlMutex));
	
		if(localPercent != -1)
			pwmValues->pwmPercent = localPercent;
		localPercent = pwmValues->pwmPercent;
	
		if(localFrequency != -1)
			pwmValues->frequency = localFrequency;
		localFrequency = pwmValues->frequency;
		
	//Unlock mutex
	pthread_mutex_unlock(&(pwmValues->pwmControlMutex));
	
	
	double localTemp;
	double localHum;
	
	//Lock mutex briefly to copy values
	pthread_mutex_lock(&(enviroValues->environmentControlMutex));
	
		localTemp = enviroValues->temperature;
		localHum = enviroValues->humidity;
	
	//Unlock mutex
	pthread_mutex_unlock(&(enviroValues->environmentControlMutex));

	std::ostringstream responseContent;
	responseContent.precision(2);

	//Generate XML for response
	responseContent << "<?xml version='1.0' encoding='utf-8'?>";
	responseContent << "\n";

	responseContent << "<data>";
		responseContent << "\n";
	
		//Generate percent response XML
		responseContent << "<percent>";
		responseContent << localPercent;
		responseContent << "</percent>";
		responseContent << "\n";
		
		//Generate frequency response XML
		responseContent << "<frequency>";
		responseContent << localFrequency;
		responseContent << "</frequency>";
		responseContent << "\n";
		
		//Generate temperature response XML
		
		responseContent << "<temperature>";
		responseContent << std::fixed << localTemp;
		responseContent << "</temperature>";
		responseContent << "\n";
		
		//Generate humidity response XML
		responseContent << "<humidity>";
		responseContent << std::fixed << localHum;
		responseContent << "</humidity>";
		responseContent << "\n";
		
		responseContent << std::defaultfloat;

	responseContent << "</data>";

	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Length: " + std::to_string(responseContent.str().length()) + "\r\n";
	response += "Content-Type: text/xml\r\n";
	response += "Connection: Closed\r\n";
	response += "\r\n";
	response += responseContent.str();


	return response;
}

std::string errorPage() { //Return a basic HTTP 404 Error page
	std::string responseContent = "404 NOT FOUND";
	std::string response = "HTTP/1.1 404 Not Found\r\n";
	response += "Content-Length: " + std::to_string(responseContent.length()) + "\r\n";
	response += "\r\n";
	response += responseContent;

	return response;
}

std::string evaluateRequest(std::string file, std::unordered_map<std::string, std::string>& params, void* data) {

	//Scalable place to add handlers for different file requests
	//Currently, only root page and state.xml are valid
	//If file does not match any listings, return a basic HTTP 404 error page
	
	if(file.compare("/") == 0) {
		return mainPage();
	} else if(file.compare("/state.xml") == 0) {
		return statePage(params, data);
	} else if(file.compare("/index.html") == 0) { //Redundancy for more intuitive access
		return mainPage();	
	} else {
		return errorPage();
	}
}

int main() {
	
	//Declare struct for sharing values between server and threads
	struct PwmValues pwmValues;
	struct EnvironmentValues enviroValues;
	
	struct ValuesContainer valuesContainer = { &pwmValues, &enviroValues };
	
	
	//Create thread for PWM control
	pthread_t pwmTid;
	pthread_create(&pwmTid, NULL, pwmControl, &pwmValues);
	
	
	//Create thread for environment sensor control
	pthread_t enviroTid;
	pthread_create(&enviroTid, NULL, environmentControl, &enviroValues);
	
	
	
	//Declare variables for sockets
	unsigned int s;
	struct sockaddr_in local;

	//Create TCP socket
	s = socket(AF_INET, SOCK_STREAM, 0);

	//Check failure condition
	if(s == -1) {
		std::cerr << "Socket creation failed" << std::endl;
		std::cerr << "Error number: " << errno << std::endl;
		return 1;
	}

	//Clear memory of local
	std::memset(&local, 0, sizeof(local));


	local.sin_family = AF_INET; //Internet socket
	local.sin_addr.s_addr = htonl(INADDR_ANY); //Listen on any IP address
	local.sin_port = htons(8000); //Open on port 8000


	//Bind socket
	int success = bind(s, (struct sockaddr*) &local, sizeof(local));
	if(success == -1) {
		std::cerr << "Socket binding failed" << std::endl;
		std::cerr << "Error number: " << errno << std::endl;
		return 1;
	}

	//Set socket to listen to up to three connections
	success = listen(s, 3);
	if(success == -1) {
		std::cerr << "Creating passive listening socket failed" << std::endl;
		std::cerr << "Error number: " << errno << std::endl;
	}

	//Main loop
	while(true) {
		
		//Declare client
		struct sockaddr_in client;
		unsigned int clientLength = sizeof(client);

		//Accept connection to client
		//Note that accept() internally prevents the infinite while loop from spinlocking the CPU
		int client_val = accept(s, (struct sockaddr*) &client, &clientLength);



		//Declare buffer of size and clear its contents
		int buffSize = 101; //101 for 100 characters and \0 terminator
		char buffer[buffSize];
		std::memset(buffer, '\0', buffSize);


		//Populate request string based on HTTP content of values read from client connection
		std::string request;
		for(int count = read(client_val, buffer, buffSize - 1); count > 0; count = read(client_val, buffer, buffSize - 1)) { //buffSize - 1, to maintain \0 terminator
			
			//If buffer isn't completely full, add \0 terminator after last byte of data
			if(count != buffSize - 1) {
				buffer[count] = '\0';
			}

			request += buffer; //Append buffer to request string


			//Break loop when \r\n\r\n string is encountered
			//Break is necessary because otherwise read() will never return, because the socket remains open
			if(request[request.length() - 4] == '\r' && request[request.length() - 3] == '\n' && request[request.length() - 2] == '\r' && request[request.length() - 1] == '\n')
				break;

		}

		//use request's c_str for quicker char access
		const char* requestC_str = request.c_str();


		//In HTTP, the first line always looks like
		//<METHOD> <URL> HTTP/<VERSION>
		//Thus, the URL is always contained between the first and second spaces

		//Identify indeces of first and second spaces
		int firstSpace = -1;
		int secondSpace = -1;

		for(int i = 0; i < request.length(); i++) {
			if(requestC_str[i] == ' ') {
				if(firstSpace == -1) {
					firstSpace = i;
				} else {
					secondSpace = i;
					break;
				}
			}
		}


		//Identify URL from substring between spaces
		std::string URL = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);


		//Declare a hashmap to contain the variable/value pairs contained in the URL
		//These pairs form the basis of the REST-style interface
		//A REST interface was chosen to be consistent with those of other products in the VUE,
		//namely the WebRelays
		std::unordered_map<std::string, std::string> params;

		//URLs are of the form
		// /path/to/file.html?var1=a&var2=b&var3=c&...&varX=x

		//Question mark separates file component of URL from parameter component
		int questionPos = URL.find("?");
		
		//Declare string to hold file component of URL
		std::string file;

		if(questionPos != std::string::npos) { //If the string contains a question mark
			//File component is everything before the question mark
			file = URL.substr(0, questionPos);

			//Declar variables to hold positions between distinct parameters
			int parStart = questionPos + 1;
			int parEnd = 0;

			while(parStart < URL.length()) {
				int parEnd = URL.find('&', parStart); //Identify & inidcating end of parameter, after the end of the last parameter
				if(parEnd == std::string::npos) //If it is not in the string, this must be the last parameter
					parEnd = URL.length();

				std::string par = URL.substr(parStart, parEnd - parStart); //Get parameter substring

				int eqPos = par.find("="); //Equal sign separarates variable from value

				if(eqPos != std::string::npos) { //Minor error handling, ensure that parameter string contains =
					std::string variable = par.substr(0, eqPos); //Set variable to equal variable component of parameter
					std::string value = par.substr(eqPos + 1, par.length() - eqPos); //Set value to equal value component of parameter
					params.emplace(variable, value); //Add variable, value pair to parameter hashmap
				}

				parStart = parEnd + 1; //Set the parameter start index for the next iteration to be 1 after this iteration's end index

			}
		} else { //Else the string does not contain a question mark, thus only contains a file
			file = URL;
		}

		std::string response = evaluateRequest(file, params, &valuesContainer); //Call evaluate request, which handles the HTTP request and returns response content

		write(client_val, response.c_str(), response.length()); //Send response to the client

		close(client_val); //Close the client
	}
}