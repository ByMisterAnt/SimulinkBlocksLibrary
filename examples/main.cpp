#include <iostream>

#include "../include/SimulinkBlocksLibrary.hpp"


using namespace SimulinkBlock;

int main()
{
    IntegratorBlock<double> integrator;
    SaturationBlock<double> saturation{-10, 10};

    for (int i = 0; i < 100; i++)
    {
        integrator.step(1, 0.2);
        saturation.step( integrator.getOutput() );

        std::cout << "integrators output: \t"
                  << integrator.getOutput()
                  << '\t'
                  << "saturations output: \t"
                  << saturation.getOutput()
                  << std::endl;
    }
    return 0;
}
