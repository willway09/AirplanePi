#include "PhysicalController.hpp"
#include "SFML/Window/Joystick.hpp"

#include <iostream>
#include <string>

const std::string PhysicalController::buttonName[] = {"A","B","X","Y","LB","RB","MENU","START","XBOX","LSB","RSB"};

PhysicalController::PhysicalController(unsigned int port, unsigned int mode){
    this->setMode(mode);
    this->setPort(port);

    sf::Joystick::update();
    if(sf::Joystick::isConnected(port)) std::cout << "Controller (Port #" << port << " Mode #" << mode << ") connected" <<std::endl;
    else std::cout << "Connection failed" << std::endl;
}

PhysicalController::~PhysicalController(){
    std::cout << "Controller destructed" << std::endl;
}

void PhysicalController::setMode(unsigned int mode){
    if(mode>9) this->mode = 0; //assures all other channel arguments default to 0
    else this->mode = mode;
}

void PhysicalController::setPort(unsigned int port){
    if(port>8) this->port = 0; //assures invalid port arguments default to 0
    else this->port = port;
}

bool PhysicalController::getButton(unsigned int buttonNumber){
    sf::Joystick::update();
    if(sf::Joystick::isButtonPressed(this->port,buttonNumber)) return true;
    else return false;
}

bool PhysicalController::getButton(std::string buttonString){
    for(int x=0;x<11;x++){
        if(buttonName[x].compare(std::string(buttonString)) == 0){
            sf::Joystick::update();
            if(sf::Joystick::isButtonPressed(this->port,x)) return true;
        }
    }
    return false;
}

float PhysicalController::getThrust(){
    if(cruiseControl){
        return this->cruiseControlThrust;
    }
    else{
        float thrust  = 0;
        sf::Joystick::update();
        switch(mode){
            case 0:
            case 1:
                thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::V);
                thrust-=(2*thrust); //thrust values need to be reversed because of how SFML is designed
                if(thrust<20) return 0; //cannot have negative thrust
                else return thrust/100 * .23;
            case 2:
            case 3:
                thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::Y);
                thrust-=(2*thrust); //thrust values need to be reversed because of how SFML is designed
                if(thrust<20) return 0; //cannot have negative thrust
                else return thrust/100 * .23 ;
            case 4:
            case 5:
            case 9:
                thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::R);
                return (thrust+=100)/200 * .23; //thrust values from the trigger range [-100,100] because of how SFML is designed... need to be from [0,200] then [0,100]
            case 6:
            case 7:
            case 8:
                thrust = sf::Joystick::getAxisPosition(this->port, sf::Joystick::Z);
                return (thrust+=100)/200 * .23; //thrust values from the trigger range [-100,100] because of how SFML is designed... need to be from [0,200] then [0,100]
            default:
                std::cout << "Error: thrust" << std::endl;
                return 0;
        }
    }

}

float PhysicalController::getPitch(){
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

float PhysicalController::getRoll(){
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


float PhysicalController::getYaw(){
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

float PhysicalController::getMotor(){
    return this->getThrust();
}

float PhysicalController::getElevator(){
    return this->getPitch();
}

float PhysicalController::getAileron(){
    return this->getRoll();
}

float PhysicalController::getRudder(){
    return this->getYaw();
}

float PhysicalController::toggleLeftHandMode(){
    sf::Joystick::update();
    if(sf::Joystick::getAxisPosition(this->port, sf::Joystick::PovX)==-100){
        this->setMode(8);
    }
}

float PhysicalController::toggleRightHandMode(){
    sf::Joystick::update();
    if(sf::Joystick::getAxisPosition(this->port, sf::Joystick::PovX)==100){
        this->setMode(9);
    }
}

float PhysicalController::toggleCruiseControll(){
    float prevCruiseControlCounter = this->cruiseControlCounter;
    sf::Joystick::update();
    switch(mode){
        case 0:
        case 1:
        case 4:
        case 5:
        case 8:
            if(sf::Joystick::isButtonPressed(this->port,9)){
                this->cruiseControlCounter++;
            }
            else{
                this->cruiseControlCounter = 0;
            }
            break;
        case 2:
        case 3:
        case 6:
        case 7:
        case 9:
            if(sf::Joystick::isButtonPressed(this->port,10)){
                this->cruiseControlCounter++;
            }
            else{
                this->cruiseControlCounter = 0;
            }
            break;
    }
    if(cruiseControlCounter==0 && prevCruiseControlCounter>0){
        std::cout << "cruise control toggled" << std::endl;
        this->cruiseControlThrust = this->getThrust();
        cruiseControl = !cruiseControl;
    }
}

void PhysicalController::listenForToggles(){
    this->toggleLeftHandMode();
    this->toggleRightHandMode();
    this->toggleCruiseControll();
}

float PhysicalController::listButtons(){
    std::cout << "";
    for(int x=0;x<32;x++){
        sf::Joystick::update();
        if(sf::Joystick::isButtonPressed(0,x)){
            std::cout << buttonName[x] << " ";
        }
    }
    std::cout << std::endl;
}


void PhysicalController::update() {
    listenForToggles();
}

Controller::Type PhysicalController::getType() {
    return Controller::PHYSICAL;
}
