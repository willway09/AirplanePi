#ifndef _WEBCONTROLLER_HPP
#define _WEBCONTROLLER_HPP

#include "Controller.hpp"

class WebController : public Controller {
    public:

        
        float getThrust() { return thrust; }
        float getPitch() { return pitch; }
        float getRoll() { return roll; }
        float getYaw() { return yaw; }

        float getMotor() { return thrust; }
        float getElevator() { return pitch; }
        float getAileron() { return roll; } 
        float getRudder() { return yaw; }
        
        void update() { }
        
        WebController::Type getType() { return Controller::WEB; } 
        
        
        
        WebController() : thrust(0), pitch(0), roll(0), yaw(0) { }
        ~WebController() { } //Do nothing
        
        void setThrust(float val) { thrust = val; }
        void setPitch(float val) { pitch = val; }
        void setRoll(float val) { roll = val; }
        void setYaw(float val) { yaw = val; }
    
    private:
        float thrust;
        float pitch;
        float roll;
        float yaw;
    
};


#endif 