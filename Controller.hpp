#include <string>

class Controller{
    public:
        Controller(unsigned int mode = 3, unsigned int port = 0); //constructor input mode and port number (default 3 and 0)
        ~Controller(); //deconstructor

        void setMode(unsigned int mode); //mutator for mode
        void setPort(unsigned int port); //mutator for port

        bool getButton(unsigned int buttonNumber); //input button ID, returns whether it's pressed or not
        bool getButton(std::string buttonString); //input button name, returns whether it's pressed or not

        float getThrust(); //returns thrust value from 0.2-1
        float getPitch(); //returns pitch value from 0.2-1
        float getRoll(); //returns roll value from 0.2-1
        float getYaw(); //returns yaw value from 0.2-1

        float getMotor();
        float getElevator();
        float getAileron();
        float getRudder();

        float toggleLeftHandMode();
        float toggleRightHandMode();

        float printPov();

    private:
        unsigned int port;
        unsigned int mode;
        static const std::string buttonName[11];
};
