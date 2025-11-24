#include <iostream>
#include "pico.h"

int main(double total_time_ms, double movement_time_ms, int target_x, int target_y)
{
    // example of usage
    PicoController pico("your com port here");
    if (pico.Connect()) {
        int scaled_x = target_x * 3;
        int scaled_y = target_y * 3;

        std::cout << "sending move: " << scaled_x << "px, " << scaled_y << "px" << std::endl;

        pico.SendMouseCommand((int)total_time_ms, (int)movement_time_ms, scaled_x, scaled_y);
        pico.Disconnect();
    }
}