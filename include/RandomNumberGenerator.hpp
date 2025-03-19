#pragma once

#include <mutex>
#include <random>

namespace SimulinkBlock
{
/**
 * @brief Класс для генерации случайных чисел
 *
 * @tparam T Тип генерируемого значения
 */
template <typename T>
class RandomNumberGenerator
{
private:
    std::mutex mtx;         //!< Мьютекс для блокировки одновременного доступа к переменным класса
    std::mt19937 generator; //!< Генератор случайных чисел Mersenne Twister
    T output = T(0);        //!< Переменная для хранения сгенерированного случайного числа

public:
    /**
     * @brief Конструктор, инициализирующий генератор случайных чисел
     */
    RandomNumberGenerator()
        : generator(std::random_device()())
    {
    }

    /**
     * @brief Функция для генерации случайного числа
     */
    void step()
    {
        std::lock_guard<std::mutex> lock(mtx);
        output = std::generate_canonical<T, 10>(generator);
    }

    /**
     * @brief Получить указатель на выходные данные блока дифференцирования
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
