#include "Controller.hpp"
#include "SFML/Window/Joystick.hpp"

#include <iostream>
#include <string>

const std::string Controller::buttonName[] = {"A","B","X","Y","LB","RB","MENU","START","XBOX","LSB","RSB"};

Controller::Controller(unsigned int mode, unsigned int port){
    this->setMode(mode);
    this->setPort(port);

    sf::Joystick::update();
    if(sf::Joystick::isConnected(port)) std::cout << "Connection succeeded" << std::endl;
    else std::cout << "Connection failed you silly" << std::endl;
    std::cout << "Port: " << this->port << std::endl;
    std::cout << "Mode: " << this->mode << std::endl;
    std::cout << "Connection succeeded" << std::endl;
}

Controller::~Controller(){
    std::cout << "Controller deconstructed" << std::endl;
}

void Controller::setMode(unsigned int mode){
    if(mode>9) this->mode = 0; //assures all other channel arguments default to 0
    else this->mode = mode;
}

void Controller::setPort(unsigned int port){
    if(port>8) this->port = 0; //assures invalid port arguments default to 0
    else this->port = port;
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
    sf::Joystick::update();
    switch(mode){
        case 0:
        case 1:
            thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::V);
            thrust-=(2*thrust); //thrust values need to be reversed because of how SFML is designed
            if(thrust<20) return 0; //cannot have negative thrust
            else return thrust/100;
        case 2:
        case 3:
            thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::Y);
            thrust-=(2*thrust); //thrust values need to be reversed because of how SFML is designed
            if(thrust<20) return 0; //cannot have negative thrust
            else return thrust/100;
        case 4:
        case 5:
        case 9:
            thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::R);
            return (thrust+=100)/200; //thrust values from the trigger range [-100,100] because of how SFML is designed... need to be from [0,200] then [0,100]
        case 6:
        case 7:
        case 8:
            thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::Z);
            return (thrust+=100)/200; //thrust values from the trigger range [-100,100] because of how SFML is designed... need to be from [0,200] then [0,100]
        default:
            std::cout << "Error: thrust" << std::endl;
            return 0;
    }
}

float Controller::getPitch(){
    float pitch = 0;
    sf::Joystick::update();
    switch(mode){
        case 0:
        case 1:
        case 4:
        case 5:
        case 8:
            pitch = sf::Joystick::getAxisPosition(this->port, sf::Joystick::Y);
            if(pitch<20 && pitch>-20) return 0;
            else return pitch/100;
        case 2:
        case 3:
        case 6:
        case 7:
        case 9:
            pitch = sf::Joystick::getAxisPosition(this->port, sf::Joystick::V);
            if(pitch<20 && pitch>-20) return 0;
            else return pitch/100;
        default:
            std::cout << "Error: pitch" << std::endl;
            return 0;
    }
}

float Controller::getRoll(){
    float roll = 0;
    sf::Joystick::update();
    switch(mode){
        case 0:
        case 1:
        case 4:
        case 5:
        case 8:
            roll = sf::Joystick::getAxisPosition(this->port, sf::Joystick::X);
            if(roll<20 && roll>-20) return 0;
            else return roll/100;
        case 2:
        case 3:
        case 6:
        case 7:
        case 9:
            roll = sf::Joystick::getAxisPosition(this->port, sf::Joystick::U);
            if(roll<20 && roll>-20) return 0;
            else return roll/100;
        default:
            std::cout << "Error: roll" << std::endl;
            return 0;
    }

}


float Controller::getYaw(){
    float yaw = 0;
    sf::Joystick::update();
    switch(mode){
        case 0:
        case 3:
        case 4:
        case 7:
        case 8:
            yaw = sf::Joystick::getAxisPosition(this->port, sf::Joystick::X);
            if(yaw<20 && yaw>-20) return 0;
            else return yaw/100;
        case 1:
        case 2:
        case 5:
        case 6:
        case 9:
            yaw = sf::Joystick::getAxisPosition(this->port, sf::Joystick::U);
            if(yaw<20 && yaw>-20) return 0;
            else return yaw/100;
        default:
            std::cout << "Error: yaw" << std::endl;
            return 0;
    }

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

float Controller::toggleLeftHandMode(){
    if(sf::Joystick::getAxisPosition(this->port, sf::Joystick::PovX)==-1){
        this->setMode(8);
    }
}

float Controller::toggleRightHandMode(){
    if(sf::Joystick::getAxisPosition(this->port, sf::Joystick::PovX)==1){
        this->setMode(9);
    }
}
