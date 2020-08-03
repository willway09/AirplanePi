#include "Controller.hpp"
#include "SFML/Window/Joystick.hpp"

#include <iostream>
#include <string>


Controller::Controller(unsigned int channels, unsigned int port){
    if(channels!=4) this->channels = 3; //assures all other channel arguments default to 3 (easy), unless client wants 4 (hard)
    else this->channels = 4;
    if(port>8) this->port = 0; //assures invalid port arguments default to 0
    else this->port = port;
    sf::Joystick::update();
    if(sf::Joystick::isConnected(port)) {
        std::cout << "Connection succeeded" << std::endl;
    }
    else{
        std::cout << "Connection failed you silly" << std::endl;
    }
}

Controller::~Controller(){
    std::cout << "Controller deconstructed" << std::endl;
}

Controller::setChannels(unsigned int channels){
    if(channels!=4) this->channels = 3; //assures all other channel arguments default to 3 (easy), unless client wants 4 (hard)
    else this->channels = 4;
}

bool Controller::getButton(unsigned int buttonNumber){
    sf::Joystick::update();
    if(sf::Joystick::isButtonPressed(this->port,buttonNumber)) return true;
    else return false;
}

bool Controller::getButton(std::string buttonString){
    for(int x=0;x<11;x++){
        if(buttonName[x].compare(std::string(buttonString)) == 0){
            sf::Joystick::update();
            if(sf::Joystick::isButtonPressed(this->port,x)) return true;
        }
    }
    return false;
}

float Controller::getThrust(){
    float thrust = 0;
    switch(channels){
        case 3:
        case 4:
            thrust = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
    }
    if(thrust<20) return 0;
    else return thrust/100;
}

float Controller::getPitch(){
    float pitch = 0;
    switch(channels){
        case 3:
        case 4:
            pitch = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    }
    if(pitch<20) return 0;
    else return pitch/100;
}

float Controller::getRoll(){
    float roll = 0;
    switch(channels){
        case 3:
        case 4:
            roll = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    }
    if(roll<20) return 0;
    else return roll/100;
}


float Controller::getYaw(){
    float yaw = 0;
    switch(channels){
        case 3:
            yaw = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        case 4:
            yaw = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
    }
    if(yaw<20) return 0;
    else return yaw/100;
}

float Controller::getMotor(){
    return this->getThrust();
}

float Controller::getElevator(){
    return this->getPitch();
}

float Controller::getAileron(){
    return this->getRoll();
}

float Controller::getRudder(){
    return this->getYaw();
}
