#pragma once

#include <cmath>

namespace SimulinkBlock
{
/**
 * @brief Класс генератора синусоидного сигнала
 *
 * @tparam T Тип выходного значения
 * @tparam U Тип параметров синусоиды
 */
template <typename T, typename U>
class SineWaveGenerator
{
private:
    U amplitude;     //!< Амплитуда синусоиды
    U frequency;     //!< Частота синусоиды
    U phase;         //!< Сдвиг фазы синусоиды
    T output = T(0); //!< Выходное значение

public:
    /**
    * @brief Конструктор класса SineWaveGenerator
    *
    * @param amp Амплитуда синусоиды
    * @param freq Частота синусоиды
    * @param ph Сдвиг фазы синусоиды
    */
    SineWaveGenerator(U amp = U(1), U freq = U(1), U ph = U(0))
        : amplitude{amp}, frequency{freq}, phase{ph}
    {
    }

    /**
    * @brief Выполнить один шаг генерации синусоидного сигнала
    *
    * @param time Время для генерации сигнала
    */
    void step(U time)
    {
        output = static_cast<T>(amplitude * sin(2 * M_PI * frequency * time + phase));
    }

    /**
    * @brief Настроить параметры генератора синусоиды
    *
    * @param amp Амплитуда синусоиды
    * @param freq Частота синусоиды
    * @param ph Сдвиг фазы синусоиды
    */
    void setup(U amp, U freq, U ph)
    {
        amplitude = amp;
        frequency = freq;
        phase     = ph;
    }

    /**
     * @brief Получить указатель на выходные данные блока синусоиды
     *
     * @return Указатель на выходные данные
     */
    const T& getOutput() const
    {
        return output;
    }

    /**
     * @brief Обнулить текущие настройки блока
     */
    void reset()
    {
        amplitude = U(1);
        frequency = U(1);
        phase     = U(0);
        output    = U(0);
    }
};
}
