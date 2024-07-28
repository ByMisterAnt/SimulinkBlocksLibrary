#pragma once

#include <array>
#include <algorithm>


namespace SimulinkBlock
{
/**
 * @brief Класс для работы с одномерной таблицей поиска
 *
 * @tparam T Тип данных массива и возвращаемого значения
 * @tparam N Тип размера массива
 */
template <typename T, std::size_t N>
class LookupTable1D
{
private:
    std::array<T, N> tableInputs;  //!< Входные значения таблицы
    std::array<T, N> tableOutputs; //!< Выходные значения таблицы
    T output = T(0);               //!< Значение, экстраполированное из таблицы

public:
    /**
     * @brief Конструктор класса LookupTable1D.
     * @param inputs Вектор входных значений.
     * @param outputs Вектор соответствующих выходных значений.
     */
    LookupTable1D(const std::array<T, N>& inputs, const std::array<T, N>& outputs)
        : tableInputs{inputs}, tableOutputs{outputs}
    {
    }

    /**
     * @brief Интерполяция значения на основе входного значения.
     * @param inputValue Входное значение для интерполяции.
     */
    void interpolate(const T& inputValue)
    {
        if (inputValue < tableInputs.front() || inputValue > tableInputs.back())
        {
            // Значение находится за пределами таблицы, поэтому экстраполировать
            extrapolate(inputValue);
        }
        else
        {
            auto it = std::upper_bound(tableInputs.begin(), tableInputs.end(), inputValue);
            if (it == tableInputs.begin())
            {
                output = tableOutputs.front();
            }
            if (it == tableInputs.end())
            {
                output = tableOutputs.back();
            }

            size_t idx = it - tableInputs.begin();
            T x0 = tableInputs[idx - 1];
            T x1 = tableInputs[idx];
            T y0 = tableOutputs[idx - 1];
            T y1 = tableOutputs[idx];

            // Линейная интерполяция
            output = y0 + (y1 - y0) * (inputValue - x0) / (x1 - x0);
        }
    }

    /**
     * @brief Экстраполяция значения на основе входного значения
     * @param inputValue Входное значение для экстраполяции
     */
    void extrapolate(const T& inputValue)
    {
        if (inputValue < tableInputs.front())
        {
            // Линейная экстраполяция для значений ниже таблицы
            T x0 = tableInputs.front();
            T x1 = tableInputs[1];
            T y0 = tableOutputs.front();
            T y1 = tableOutputs[1];
            output = y0 + (y1 - y0) * (inputValue - x0) / (x1 - x0);
        }
        else if (inputValue > tableInputs.back())
        {
            // Линейная экстраполяция для значений выше таблицы
            T x0 = tableInputs[tableInputs.size() - 2];
            T x1 = tableInputs.back();
            T y0 = tableOutputs[tableOutputs.size() - 2];
            T y1 = tableOutputs.back();
            output = y0 + (y1 - y0) * (inputValue - x0) / (x1 - x0);
        }
        else
        {
            // Значение находится в пределах таблицы, поэтому интерполировать
            interpolate(inputValue);
        }
    }

    /**
     * @brief Получить указатель на текущее состояние блока интегрирования
     *
     * @return Указатель на текущее значение, экстраполированное из таблицы
     */
    const T& getOutput() const
    {
        return output;
    }

    /**
     * @brief Обнулить текущий выход блока
     */
    void reset()
    {
        output = T(0);
    }
};
}
