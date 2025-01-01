#pragma once

#include <mutex>
#include <stdexcept>


namespace SimulinkBlock
{
/**
  * @brief Класс, реализующий блок ограничения скорости изменения
  *
  * @tparam T Тип данных для операций ограничения
  */
template <typename T>
class RateLimiter {
private:
    std::mutex mtx; //!< Мьютекс для блокировки одновременного доступа к переменным класса
    T risingLimit;  //!< Предел увеличения
    T fallingLimit; //!< Предел уменьшения
    T state = T(0); //!< Текущее состояние

public:
    /**
     * @brief Конструктор для инициализации блока ограничения скорости изменения
     *
     * @param rLim Предел увеличения
     * @param fLim Предел уменьшения
     */
    RateLimiter(T rLim, T fLim)
        : risingLimit{rLim},
          fallingLimit{fLim}
    {
        if(fallingLimit > risingLimit)
        {
            std::swap(risingLimit, fallingLimit);
            throw std::invalid_argument("Min value should not be greater than max value");
        }
    }

    /**
     * @brief Выполнить один шаг расчёта блока
     *
     * @param input Входное значение блока
     */
    void step(const T& input, const T& dt)
    {
        std::lock_guard<std::mutex> lock(mtx);
        // Скорость изменения сигнала
        T rate = input - state;

        if (rate > risingLimit * dt)
        {
            state = state + risingLimit * dt;
        }

        else if (rate < fallingLimit * dt)
        {
            state = state + fallingLimit * dt;
        }

        else
        {
            state = input;
        }
    }

    /**
     * @brief Установить состояние блока
     *
     * @param newState Новое значение состояния для установки
     */
    void setState(const T& newState)
    {
        std::lock_guard<std::mutex> lock(mtx);
        state = newState;
    }

    /**
     * @brief Ссылка на текущее состояние блока
     *
     * @return Указатель на текущее состояние
     */
    const T& getOutput()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return state;
    }

    /**
     * @brief Обнулить текущее состояние блока
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);
        state = T(0);
    }
};
}
