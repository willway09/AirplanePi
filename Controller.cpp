#include "Controller.hpp"

#include "SFML/Window/Joystick.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <map>

Controller::Controller(unsigned int port){
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
    if(sf::Joystick::isButtonPressed(0,0)){
        return true;
    }
    else return false;
}
