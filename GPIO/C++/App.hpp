
#ifndef APP_H
#define APP_H


#include <iostream>
#include <fstream>

class _PsudoDriver
{   
    private:
        std::string const PATH = "/dev/SamiGPIO_Dev";
        std::fstream fd; 
    public:
        void __Write(std::string __Buffer);
        std::string __Read();
};

class _LED
{
    private:
        _PsudoDriver Driver;
    public:
        void LED_ON();
        void LED_OFF();

};

class Display
{
    public:
        static void DisplayOnChange(std::string value);
};
#endif /* APP_H */