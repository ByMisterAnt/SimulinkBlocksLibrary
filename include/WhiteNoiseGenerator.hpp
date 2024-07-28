#pragma once

#include <random>


namespace SimulinkBlock
{
/**
 * @brief Шаблон класса для генерации белого шума типа T
 */
template <typename T>
class WhiteNoiseGenerator
{
private:
    std::mt19937 generator;                   //!< Генератор случайных чисел Mersenne Twister
    std::normal_distribution<T> distribution; //!< Нормальное распределение для генерации белого шума
    T output = T(0);                          //!< Переменная для хранения сгенерированного значения белого шума

public:
    /**
     * @brief Конструктор, инициализирующий генератор случайных чисел и нормальное распределение
     */
    WhiteNoiseGenerator(T mean, T stddev)
        : generator(std::random_device()()),
        distribution(mean, stddev)
    {
    }

    /**
     * @brief Функция для генерации белого шума
     */
    void step()
    {
        output = distribution(generator);
    }

    /**
     * @brief Получить указатель на выходные данные блока генерации белого шума
     *
     * @return Указатель на выходные данные
     */
    const T& getOutput() const
    {
        return output;
    }

    /**
     * @brief Обнулить текущее состояние блока
     */
    void reset()
    {
        output = T(0);
    }
};
}
