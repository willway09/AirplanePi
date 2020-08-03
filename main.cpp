#include "SFML/Window/Joystick.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <map>

#include "Controller.hpp"

int main(){
    Controller xbox(3,0);

    float timeOut = 0.1;
    while(true){
        std::cout << "Thrust: " << xbox.getThrust() << " ";
        std::cout << "Pitch: " << xbox.getPitch() << " ";
        std::cout << "Roll: " << xbox.getRoll() << " ";
        std::cout << "Yaw: " << xbox.getYaw() << " " << std::endl;
        usleep(timeOut*1000000);
    }
}
