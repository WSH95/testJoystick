#include "read_joystick.hpp"
#include "Timer.h"
#include <iostream>

int main()
{
    ReadJoystick js;
    Timer tt;

    bool flag = false;

    do
    {
        flag = js.init();

        while (flag)
        {
            flag = js.read_event();

            tt.start();
            js.updateJsData();
            std::cout << "time spent1: " << tt.getMs() << "ms" << std::endl;

            tt.start();
            js.lcmPublish();
//            std::cout << "time spent2: " << tt.getMs() << "ms" << std::endl;
            usleep(1000);
        }
        sleep(1);
    }
    while (!flag);

    return 0;
}