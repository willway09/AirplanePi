#ifndef _PHYSICALCONTROLLER_HPP
#define _PHYSICALCONTROLLER_HPP

#include "Controller.hpp"

class PhysicalController : public Controller {
    public:
    
        //From Controller
    
    
         float getThrust(); //returns thrust value from 0.2-1
         float getPitch(); //returns pitch value from 0.2-1
         float getRoll(); //returns roll value from 0.2-1
         float getYaw(); //returns yaw value from 0.2-1

         float getMotor();
         float getElevator();
         float getAileron();
         float getRudder();
        
         void update();
         
        Controller::Type getType();
    
        //This class
    
        PhysicalController(unsigned int port, unsigned int mode = 3); //constructor input mode and port number (default 3 and 0)
        ~PhysicalController(); //deconstructor
    
    
        void setMode(unsigned int mode); //mutator for mode
        void setPort(unsigned int port); //mutator for port
    
        bool getButton(unsigned int buttonNumber); //input button ID, returns whether it's pressed or not
        bool getButton(std::string buttonString); //input button name, returns whether it's pressed or not
        
        
        void listenForToggles(); //calls all toggle methods so server always only calls one method 
    
            float toggleLeftHandMode();
            float toggleRightHandMode();
            
            float toggleCruiseControll();
    
        float listButtons();
    
    private:
        unsigned int port;
        unsigned int mode;
        
        bool cruiseControl = false;
        float cruiseControlThrust;
        unsigned int cruiseControlCounter;
        
        static const std::string buttonName[11];
};

#endif //_PHYSICALCONTROLLER_HPP