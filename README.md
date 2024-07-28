# SimulinkBlocks

Легковесная Header-only библиотека, реализующая логику работы некоторых блоков, используемых в Simulink/Scilab. 
В настоящее время готовы блоки:
* Lookup Table 1-D
* Integrator
* Derivative
* Rate Limiter
* White Noise
* Sine Wave
* Triggered Subsystem
* Random Number
* Saturation

## Установка

Для установки необходимо скачать библиотеку и установить cmake (для запуска примеров и тестов)
```
git clone --recurse-submodules https://github.com/ByMisterAnt/SimulinkBlocksLibrary.git
```
Ubuntu:
```
sudo apt install cmake
```
Arch Linux:
```
sudo pacman -S cmake
```

## Подключение библиотеки

Для работы с библиотекой достаточно подключить заголовочный файл SimulinkBlocksLibrary.hpp из дирректории include/

## Пример кода

Ниже приведён пример кода, в котором реализовано последовательное соединение блока интегрирования и блока насыщения.
На вход блока интегрирования подаётся постоянное значение. Затем оно ограничивается в блоке насыщения.

```C++
#include <iostream>

#include "include/SimulinkBlocksLibrary.hpp"


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
```

Необходимо заметить, что в блоке интегрирования есть встроенная логика ограничения выходного значения. 
Однако могут возникать задачи, когда необходимо не ограничевать его выход
