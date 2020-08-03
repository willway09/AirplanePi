#include "SFML/Window/Joystick.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <map>

std::map<int, std::string> buttonMap;
float timeOut = 0.5;

void generateButtonMap(){
    std::string buttonName[11] = {"A","B","X","Y","LB","RB","MENU","START","XBOX","LSB","RSB"};
    for(int x=0; x<10; x++){
        buttonMap.insert(std::pair<int,std::string>(x,buttonName[x]));
    }
}

void initialization(){
    unsigned int buttonCount = sf::Joystick::getButtonCount(0);
    std::cout << buttonCount << std::endl;

    std::string axis = sf::Joystick::hasAxis(0, sf::Joystick::X) ? "yes" : "no";
    std::cout << "Controller axis: " << axis << std::endl;
}

void joysticks(){
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
        std::cout << "x: " << " " << x << " " << " y: " << " " << y << " " << " ";
        std::cout << "z: " << " " << z << " " << " r: " << " " << r << " " << " ";
        std::cout << "u: " << " " << u << " " << " v: " << " " << v << " " << " ";
        std::cout << "povx: " << " " << povx << " " << " povy: " << " " << povy << " " << std::endl;

        usleep(timeOut*1000000);
    }
}

void counter(){
    unsigned int c = 0; //counter
    unsigned int d = 0; //cooldown
    while(true){
        if(sf::Joystick::isButtonPressed(0,0)) c++;
        else d++;

        if(d>20){
            c=0;
            d=0;
        }

        std::cout << "Counter: " << c << std::endl;

        usleep(timeOut*1000000);
    }
}

void buttons(){
    while(true){
        std::cout << "ran: ";
        for(int x=0;x<32;x++){
            sf::Joystick::update();
            if(sf::Joystick::isButtonPressed(0,x)){
                std::cout << "SUCSESS AT: " << x;
            }
        }
        std::cout << std::endl;

        usleep(timeOut*1000000);
    }
}

int main() {

    sf::Joystick::update();

    if(sf::Joystick::isConnected(0)) {
        std::cout << "Connected" << std::endl;

        generateButtonMap();
        buttons();

    }
    else{
        std::cout << "Connection failed you silly" << std::endl;
    }
}

/**/
