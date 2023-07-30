#include "App.hpp"
int flag = 1;
void _PsudoDriver::__Write(std::string __Buffer)
{
    fd.open(PATH ,std::ios::out);
    fd.write(__Buffer.c_str() , __Buffer.size());
    fd.close();
}
std::string _PsudoDriver::__Read()
{
    std::string Msg;
    fd.open(PATH ,std::ios::in);
    std::getline(fd,Msg);
    fd.close();
    return Msg;
}
void _LED::LED_ON()
{
    Driver.__Write("1");
}
void _LED::LED_OFF()
{
    Driver.__Write("0");
}

void Display::DisplayOnChange(std::string value)
{
    if(value == "1" && flag == 0)
    {
        std::cout << "Led is on" <<std::endl;
        flag=1;
    }
    else if(value == "0" && flag == 1)
    {
        std::cout << "Led is off" <<std::endl;
        flag =0;
    }
    else
    {

    }
}