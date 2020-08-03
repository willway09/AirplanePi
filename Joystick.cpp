#include "SFML/Window/Joystick.hpp"
#include <iostream>
#include <string>
#include <unistd.h>


int main() {

    sf::Joystick::update();

    if(sf::Joystick::isConnected(0)) {
        std::cout << "Connected" << std::endl;
        unsigned int buttonCount = sf::Joystick::getButtonCount(0);
        std::cout << buttonCount << std::endl;

        std::string axis = sf::Joystick::hasAxis(0, sf::Joystick::X) ? "yes" : "no";
        std::cout << "Controller axis: " << axis << std::endl;

        float timeOut = 0.5; //seconds
        while(true){
            sf::Joystick::update();
            float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
            float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
            float z = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);
            float r = sf::Joystick::getAxisPosition(0, sf::Joystick::R);
            float u = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
            float v = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
            float povx = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);
            float povy = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);
            std::cout << "x: " << std::setw(6) << x << std::setw(6) << " y: " << std::setw(6) << y << std::setw(6) << " ";
            std::cout << "z: " << std::setw(6) << z << std::setw(6) << " r: " << std::setw(6) << r << std::setw(6) << " ";
            std::cout << "u: " << std::setw(6) << u << std::setw(6) << " v: " << std::setw(6) << v << std::setw(6) << " ";
            std::cout << "povx: " << std::setw(6) << povx << std::setw(6) << " povy: " << std::setw(6) << povy << std::setw(6) << std::endl;

            usleep(timeOut*1000000);
        }
    }
    else{
        std::cout << "Connection failed you dipshit" << std::endl;
    }
}

/*for(int x=0;x<32;x++){
    sf::Joystick::update();
    if(sf::Joystick::isButtonPressed(0,x)){
        std::cout << x << "  ";
    }
}*/
