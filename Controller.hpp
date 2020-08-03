#include <string>

class Controller{
    public:
        Controller(unsigned int port); //constructor input port number (default 0)
        ~Controller();
        bool getButton(unsigned int buttonNumber); //input button ID, returns whether it's pressed or not
        bool getButton(std::string buttonString); //input button name, returns whether it's pressed or not
        float getThrust();  //returns thurst value from 0.2-1
        float getPitch(); //returns pitch value from 0.2-1
        float getRoll(); //returns roll value from 0.2-1
        float getYaw(); //returns yaw value from 0.2-1
    private:
        unsigned int port;
        std::string buttonName[11] = {"A","B","X","Y","LB","RB","MENU","START","XBOX","LSB","RSB"};
};
