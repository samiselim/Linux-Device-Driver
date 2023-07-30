#include "App.hpp"

int main()
{
    std::string input;
    _PsudoDriver D;
    _LED Led;
    
    std::cout << "************ Application Started ************" << std::endl;

    while(1)
    {
        input = D.__Read();
        if(input == "1")
        {
            Led.LED_ON();
            Display::DisplayOnChange(input);
        }
        else if(input == "0")
        {
            Led.LED_OFF();
            Display::DisplayOnChange(input);
        }
        else
        {

        }
    }
    return 0;
}