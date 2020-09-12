all: httpserver.o PhysicalController.o
	g++ -o main httpserver.o PhysicalController.o -lsfml-window -lsfml-system -lrf24 -lpthread

httpserver.o : httpserver.cpp
	g++ -c -o httpserver.o httpserver.cpp
	
PhysicalController.o : PhysicalController.hpp PhysicalController.cpp Controller.hpp
	g++ -c -o PhysicalController.o PhysicalController.cpp
	
#WebController.o : WebController.hpp WebController.cpp Controller.hpp
	#g++ -c -o WebController.o WebController.cpp
	

server: server.o
	g++ -o server server.o  -lgpiod -lpthread



clean:
	rm *.o
	rm main

cleanserver:
	rm server
