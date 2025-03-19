#pragma once

#include <mutex>
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
    std::mutex mtx;                           //!< Мьютекс для блокировки одновременного доступа к переменным класса
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
        std::lock_guard<std::mutex> lock(mtx);
        output = distribution(generator);
    }

    /**
     * @brief Получить указатель на выходные данные блока генерации белого шума
     *
     * @return Указатель на выходные данные
     */
    const T& getOutput()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return output;
    }

    /**
     * @brief Обнулить текущее состояние блока
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);
        output = T(0);
    }
};
}
