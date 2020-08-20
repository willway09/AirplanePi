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

//Controller headers
#include <Controller.hpp>

//Structs for sharing groups of values between main() and threads
struct Axes{
	Controller* controller = nullptr;
	float thrust = 0;
	float pitch = 0;
	float roll = 0;
	float yaw = 0;
	pthread_mutex_t axesControlMutex = PTHREAD_MUTEX_INITIALIZER;
};

void* updateController(void* data){
	Axes* axes = (Axes*)data;
	while(true){
		pthread_mutex_lock(*(axes->axesControlMutex));
		axes.thrust = axes->controller.getThrust();
		axes.pitch = axes->controller.getPitch();
		axes.roll = axes->controller.getRoll();
		axes.yaw = axes->controller.getYaw();
		pthread_mutex_unlock(*(axes->axesControlMutex));
		usleep(1000);
	}
}

struct ValuesContainer{
	struct Axes* axes;
};

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
	struct Axes* axes = ((struct ValuesContainer*) data)->axes;

	int localThrust = -1;
	if(isNumber(params["thrust"]))
		localThrust = std::stoi(params["thrust"]);

	int localPitch = -1;
	if(isNumber(params["pitch"]))
		localPitch = std::stoi(params["pitch"]);

	int localRoll = -1;
	if(isNumber(params["roll"]))
		localRoll = std::stoi(params["roll"]);

	int localYaw = -1;
	if(isNumber(params["yaw"]))
		localYaw = std::stoi(params["yaw"]);


	//Lock mutex briefly to update values
	pthread_mutex_lock(&(axes->axesControlMutex));

		if(localThrust != -1)
			axes->thrust = localThrust;
		localThrust = axes->thrust;

		if(localThrust != -1)
			axes->pitch = localPitch;
		localPitch = axes->pitch;

		if(localRoll != -1)
			axes->roll = localRoll;
		localRoll = axes->roll;

		if(localYaw != -1)
			axes->yaw = localYaw;
		localYaw = axes->yaw;

	//Unlock mutex
	pthread_mutex_unlock(&(axes->axesControlMutex));

	std::ostringstream responseContent;
	responseContent.precision(2);

	//Generate XML for response
	responseContent << "<?xml version='1.0' encoding='utf-8'?>";
	responseContent << "\n";

	responseContent << "<data>";
		responseContent << "\n";

		//Generate percent response XML
		responseContent << "<thrust>";
		responseContent << std::fixed << localThrust;
		responseContent << "</thrust>";
		responseContent << "\n";

		//Generate frequency response XML
		responseContent << "<pitch>";
		responseContent << std::fixed << localPitch;
		responseContent << "</pitch>";
		responseContent << "\n";

		//Generate temperature response XML

		responseContent << "<roll>";
		responseContent << std::fixed << localRoll;
		responseContent << "</roll>";
		responseContent << "\n";

		//Generate humidity response XML
		responseContent << "<yaw>";
		responseContent << std::fixed << localYaw;
		responseContent << "</yaw>";
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

	struct Axes axes;
		Controller controller(0,0);
		axes->controller = &controller;
	struct ValuesContainer valuesContainer = { &axes };

	//Create thread for controller reading
	pthread_t controllerTid;
	pthread_create(&controllerTid, NULL, updateController, &axes);

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
