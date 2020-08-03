#include "Controller.hpp"

#include "SFML/Window/Joystick.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <map>

Controller::Controller(unsigned int port){
    this->port = port;
    sf::Joystick::update();
    if(sf::Joystick::isConnected(port)) {
        std::cout << "Connected" << std::endl;
    }
    else{
        std::cout << "Connection failed you silly" << std::endl;
    }
}

Controller::~Controller(){
    std::cout << "Controller deconstructed" << std::endl;
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
    float thrust = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
    if(thrust<20) return 0;
    else return thrust/100;
}

float Controller::getRoll(){
    float roll = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    if(roll<20) return 0;
    else return roll/100;
}

float Controller::getPitch(){
    float pitch = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    if(pitch<20) return 0;
    else return pitch/100;
}

float Controller::getYaw(){
    float yaw = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    if(yaw<20) return 0;
    else return yaw/100;
}
