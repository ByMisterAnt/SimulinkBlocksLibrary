#include <iostream>
#include <thread>

#include "../../include/Joystick/JoystickReader.hpp"

using namespace SimulinkBlock;


int main()
{
    try {
        JoystickReader reader("/dev/input/event26");

        const auto& state = reader.getOutput();

        std::cout << "Joystick successfully initialized\n";

        while (true) {
            const auto& s = reader.getOutput();
            std::cout
                << "\rX: " << s.x
                << "\t Y: " << s.y
                << "\t RZ: " << s.rz
                << "\t Throttle: " << s.throttle
                << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
