#pragma once

#include <mutex>
namespace SimulinkBlock
{
/**
 * @brief Класс, реализующий триггерную подсистему
 *
 * @tparam T Тип входных данных
 * @tparam U Тип триггера
 */
template <typename T, typename U>
class TriggeredSubsystem
{
private:
    std::mutex mtx;         //!< Мьютекс для блокировки одновременного доступа к переменным класса
    T output        = T(0); //!< Внешний выход
    U prev_state    = U(0); //!< Предыдущие состояния нулевых переходов (триггер)

public:
    /**
     * @brief Выполнить один шаг триггерной подсистемы
     *
     * @param input Внешний вход
     * @param trigger_input Внешний входной триггер
     */
    void step(const T& input, const U& trigger_input)
    {
        std::lock_guard<std::mutex> lock(mtx);
        // Проверка условия триггера и предыдущего состояния нулевого перехода
        if ( trigger_input && ( prev_state != static_cast<T>(1) ) )
        {
            // Установка выхода на основе входного значения
            output = input;
        }

        // Обновление предыдущего состояния нулевого перехода
        prev_state = trigger_input;
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
     * @brief Обнулить текущее состояние блока интегрирования
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);
        prev_state = T(0);
        output     = U(0);
    }
};
}
