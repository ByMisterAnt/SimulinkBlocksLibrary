#pragma once

#include <algorithm>
#include <mutex>
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
    std::mutex mtx; //!< Мьютекс для блокировки одновременного доступа к переменным класса
    T state;        //!< Состояние блока интегрирования
    T minLimit;     //!< Минимальный предел интегрирования
    T maxLimit;     //!< Максимальный предел интегрирования

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
        std::lock_guard<std::mutex> lock(mtx);
        if(min > max)
        {
            std::swap(minLimit, maxLimit);
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
        std::lock_guard<std::mutex> lock(mtx);
        if(min > max)
        {
            std::swap(maxLimit, minLimit);
            throw std::invalid_argument("Min value should not be greater than max value");
            return;
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
        std::lock_guard<std::mutex> lock(mtx);
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
        std::lock_guard<std::mutex> lock(mtx);
        state = newState;
    }

    /**
     * @brief Ссылка на текущее состояние блока интегрирования
     *
     * @return Ссылка на текущее состояние
     */
    const T& getOutput()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return state;
    }

    /**
     * @brief Обнулить текущее состояние блока интегрирования
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);
        state = T(0);
    }
};
}
