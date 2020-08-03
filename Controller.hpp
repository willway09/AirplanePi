#include <string>

class Controller{
    public:
      Controller(unsigned int port);
      ~Controller();
      bool getA();
    private:
      std::string buttonName[11] = {"A","B","X","Y","LB","RB","MENU","START","XBOX","LSB","RSB"};
};
