# AirplanePi
An RC airplane control system for the Raspberry Pi.

### Pipeline Description
An Xbox One controller must be plugged into the transmitting computer with a Micro USB cable. Then the C++ library SFML is used to decode the controller data. The Axes struct inside `httpserver.cpp` then abstracts the data obtained from SFML, and is concurrently written locally inside the server. The server then writes a byte stream to the RF24 instance, which then subsequently sends data to the RC Plane through the NRF24L01 radio protocol. Additionally, `httpserver.cpp` also hosts an HTML web interface for various data points, including a web controller that can be used instead of a physical controller like an Xbox controller.

### Dependencies
- **SFML** - Library for decoding controller data. Joystick class used. More info [here](https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Joystick.php).
- **RF24** - Used to send and receive radio data. More info [here](https://www.arduinolibraries.info/libraries/rf24).


