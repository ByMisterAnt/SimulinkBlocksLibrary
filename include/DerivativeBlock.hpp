#pragma once

#include <algorithm>
#include <stdexcept>


namespace SimulinkBlock
{
/**
 * @brief Класс, реализующий блок дифференцирования
 *
 * @tparam T Тип прошлого состояния и входа для блока дифференцирования
 */
template <typename T>
class DerivativeBlock
{
private:
    T prevInput;               //!< Состояние блока дифференцирования
    T derivativeOutput = T(0); //!< Выход блока дифференцирования
    T minLimit;                //!< Минимальный предел дифференцирования
    T maxLimit;                //!< Максимальный предел дифференцирования

public:
    /**
         * @brief Конструктор для инициализации блока дифференцирования с заданными пределами по умолчанию
         *
         * @param min Минимальный предел дифференцирования
         * @param max Максимальный предел дифференцирования
         * @param prevInput прошлый вход блока дифференцирования
         */
    DerivativeBlock(T min = static_cast<T>(-10000), T max = static_cast<T>(10000), T prevInput = T(0))
        : minLimit{min}, maxLimit{max}, prevInput{T(0)}
    {
        if(min > max)
        {
            throw std::invalid_argument("Min value should not be greater than max value");
        }
    }

    /**
     * @brief Установить пределы результата дифференцирования
     *
     * @param min Минимальный предел дифференцирования
     * @param max Максимальный предел дифференцирования
     */
    void setLimits(const T& min, const T& max)
    {
        if(min > max)
        {
            throw std::invalid_argument("Min value should not be greater than max value");
        }

        minLimit = min;
        maxLimit = max;
    }

    /**
     * @brief Выполнить один шаг дифференцирования
     *
     * @param input Входное значение для дифференцирования
     * @param dt Временной шаг для дифференцирования
     */
    void step(const T& input, double dt)
    {
        derivativeOutput = (input - prevInput) / dt;
        derivativeOutput = std::clamp(derivativeOutput, minLimit, maxLimit);
        prevInput = input;
    }

    /**
     * @brief Установить состояние блока дифференцирования
     *
     * @param newPrevInput Новое значение состояния для установки
     */
    void setState(const T& newPrevInput)
    {
        prevInput = newPrevInput;
    }

    /**
     * @brief Получить указатель на текущее состояние блока дифференцирования
     *
     * @return Указатель на текущее состояние
     */
    const T& getState() const
    {
        return prevInput;
    }

    /**
     * @brief Получить указатель на выходные данные блока дифференцирования
     *
     * @return Указатель на выходные данные
     */
    const T& getOutput() const
    {
        return derivativeOutput;
    }

    /**
     * @brief Обнулить текущее состояние блока дифференцирования
     */
    void reset()
    {
        prevInput        = T(0);
        derivativeOutput = T(0);
    }
};
}
