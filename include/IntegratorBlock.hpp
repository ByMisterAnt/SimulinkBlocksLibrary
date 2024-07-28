#pragma once

#include <algorithm>
#include <stdexcept>


namespace SimulinkBlock
{
/**
 * @brief Класс, реализующий блок интегрирования
 *
 * @tparam T Тип состояния и входа для блока интегрирования
 */
template <typename T>
class IntegratorBlock
{
private:
    T state; //!< Состояние блока интегрирования
    T minLimit; //!< Минимальный предел интегрирования
    T maxLimit; //!< Максимальный предел интегрирования

public:
    /**
         * @brief Конструктор для инициализации блока интегрирования с заданными пределами по умолчанию
         *
         * @param min Минимальный предел интегрирования
         * @param max Максимальный предел интегрирования
         */
    IntegratorBlock(T min = static_cast<T>(-10000),
                    T max = static_cast<T>(10000),
                    T state = T(0))
        : minLimit{min},
          maxLimit{max},
          state{T(0)}
    {
        if(min > max)
        {
            throw std::invalid_argument("Min value should not be greater than max value");
        }
    }

    /**
     * @brief Установить пределы результата интегрирования
     *
     * @param min Минимальный предел интегрирования
     * @param max Максимальный предел интегрирования
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
     * @brief Выполнить один шаг интегрирования
     *
     * @param input Входное значение для интегрирования
     * @param dt Временной шаг для интегрирования
     */
    void step(const T& input, const T& dt)
    {
        // Ограничиваем результат интегрирования
        T result = state + input * dt;
        state = std::clamp(result, minLimit, maxLimit);
    }

    /**
     * @brief Установить состояние блока интегрирования
     *
     * @param newState Новое значение состояния для установки
     */
    void setState(const T& newState)
    {
        state = newState;
    }

    /**
     * @brief Получить указатель на текущее состояние блока интегрирования
     *
     * @return Указатель на текущее состояние
     */
    const T& getOutput() const
    {
        return state;
    }

    /**
     * @brief Обнулить текущее состояние блока интегрирования
     */
    void reset()
    {
        state = T(0);
    }
};
}
