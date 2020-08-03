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

        float timeOut = 0.05; //seconds
        while(true){
            sf::Joystick::update();
            for(int x=0;x<32;x++){
                if(sf::Joystick::isButtonPressed(0,x)){
                    std::cout << x << "  ";
                }
            }
            usleep(timeOut*1000000);
        }
    }
    else{
        std::cout << "Connection failed you dipshit" << std::endl;
    }
}

/*sf::Joystick::update();
float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
float z = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);
float r = sf::Joystick::getAxisPosition(0, sf::Joystick::R);
float u = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
float v = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
float povx = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);
float povy = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);
std::cout << "x: " << x << " y: " << y << " ";
std::cout << "z: " << z << " r: " << r << " ";
std::cout << "u: " << u << " v: " << v << " ";
std::cout << "povx: " << povx << " povy: " << povy << std::endl;*/
