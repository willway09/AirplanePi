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

bool Controller::getA(){
    sf::Joystick::update();
    if(sf::Joystick::isButtonPressed(this->port,0)){
        return true;
    }
    else return false;
}

bool Controller::getB(){
    sf::Joystick::update();
    if(sf::Joystick::isButtonPressed(this->port,1)){
        return true;
    }
    else return false;
}
