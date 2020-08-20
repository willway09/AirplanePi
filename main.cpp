//run this: g++ -o main main.cpp Controller.cpp -lsfml-window

#include "SFML/Window/Joystick.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <iomanip>

#include "Controller.hpp"

int main(){
    unsigned int input_mode;
    std::cin >> input_mode;
    Controller xbox(input_mode,0);
    float timeOut = 0.1;
    while(true){
        xbox.toggleLeftHandMode();
        xbox.toggleRightHandMode();
        std::cout << "Thrust: " << std::setw(9) << xbox.getThrust() << " ";
        std::cout << "Pitch: " << std::setw(9) << xbox.getPitch() << " ";
        std::cout << "Roll: " << std::setw(9) << xbox.getRoll() << " ";
        std::cout << "Yaw: " << std::setw(9) << xbox.getYaw() << std::endl;
        xbox.printPov();
        usleep(timeOut*1000000);
    }
}
