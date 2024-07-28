#pragma once

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
        output = std::generate_canonical<T, 10>(generator);
    }

    /**
     * @brief Получить указатель на выходные данные блока дифференцирования
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
