#pragma once

#include "IntegratorBlock.hpp"
#include "DerivativeBlock.hpp"
#include <algorithm>
#include <mutex>
#include <stdexcept>


namespace SimulinkBlock
{
/**
 * @brief Класс, реализующий блок ПИД регулятора
 *
 * @tparam T Тип прошлого состояния и входа для блока
 */
template <typename T>
class PID
{
private:
    std::mutex mtx; //!< Мьютекс для блокировки одновременного доступа к переменным класса

    DerivativeBlock<T> derivative; //!< Блок дифференцирования
    IntegratorBlock<T> integrator; //!< Блок интегрирования

    T pidOutput = T{0}; //!< Выход блока ПИД регулятора

    T P = T{0}; //!< Коэффициент усиления для пропорциональной составляющей
    T I = T{0}; //!< Коэффициент усиления для интегрирующей составляющей
    T D = T{0}; //!< Коэффициент усиления для дифференцирующей составляющей

public:
    PID() = default;
    /**
     * @brief Конструктор для инициализации блока ПИД регулятора с заданными пределами по умолчанию
     * @param P Коэффициент усиления для пропорциональной составляющей
     * @param I Коэффициент усиления для интегрирующей составляющей
     * @param D Коэффициент усиления для дифференцирующей составляющей
     */
    PID(const T &p, const T &i, const T &d) : P{p}, I{i}, D{d}
    {}

    /**
     * @brief Конструктор для инициализации блока ПИД регулятора с заданными пределами по умолчанию
     *
     * @param P Коэффициент усиления для пропорциональной составляющей
     * @param I Коэффициент усиления для интегрирующей составляющей
     * @param D Коэффициент усиления для дифференцирующей составляющей
     * @param minI Минимальный предел  интегрирования
     * @param maxI Максимальный предел интегрирования
     * @param min Минимальный предел дифференцирования
     * @param max Максимальный предел дифференцирования
     */
    PID(const T &p, const T &i, const T &d,
        const T& minI, const T& maxI, const T& minD, const T& maxD)
        : P{p}, I{i}, D{d}
    {
        setLimits(minI, maxI, minD, maxD);
    }

    /**
     * @brief Установить коэффициенты для блока ПИД регулятора
     *
     * @param P Коэффициент усиления для пропорциональной составляющей
     * @param I Коэффициент усиления для интегрирующей составляющей
     * @param D Коэффициент усиления для дифференцирующей составляющей
     */
    void setCoeffs(const T &p, const T &i, const T &d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        P = p;
        I = i;
        D = d;
    }

    /**
     * @brief Установить коэффициент П для блока ПИД регулятора
     */
    void setPCoeff(const T &p)
    {
        std::lock_guard<std::mutex> lock(mtx);
        P = p;
    }

    /**
     * @brief Установить коэффициент И для блока ПИД регулятора
     */
    void setICoeff(const T &i)
    {
        std::lock_guard<std::mutex> lock(mtx);
        I = i;
    }

    /**
     * @brief Установить коэффициент Д для блока ПИД регулятора
     */
    void setDCoeff(const T &d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        D = d;
    }

    /**
     * @brief Установить пределы результата дифференцирования
     *
     * @param minI Минимальный предел  интегрирования
     * @param maxI Максимальный предел интегрирования
     * @param minD Минимальный предел  дифференцирования
     * @param maxD Максимальный предел дифференцирования
     */
    void setLimits(const T& minI, const T& maxI, const T& minD, const T& maxD)
    {
        std::lock_guard<std::mutex> lock(mtx);
        integrator.setLimits(minI, maxI);
        derivative.setLimits(minD, maxD);
    }

    /**
     * @brief Установить пределы результата интегрирования
     *
     * @param min Минимальный предел интегрирования
     * @param max Максимальный предел интегрирования
     */
    void setIntegratorLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        integrator.setLimits(min, max);
    }

    /**
     * @brief Установить пределы результата дифференцирования
     *
     * @param min Минимальный предел дифференцирования
     * @param max Максимальный предел дифференцирования
     */
    void setDerivativeLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        derivative.setLimits(min, max);
    }

    /**
     * @brief Выполнить один шаг блока ПИД регулятора
     *
     * @param input Входное значение
     * @param dt Временной шаг
     */
    void step(const T& input, double dt)
    {
        std::lock_guard<std::mutex> lock(mtx);
        integrator.step(input, dt);
        derivative.step(input, dt);
        pidOutput = P * input + integrator.getOutput() * I + derivative.getOutput() * D;
    }

    /**
     * @brief Установить состояние блока дифференцирования
     *
     * @param newPrevInput Новое значение состояния для установки
     */
    void setDerivativeState(const T& newPrevInput)
    {
        std::lock_guard<std::mutex> lock(mtx);
        derivative.setState(newPrevInput);
    }

    /**
     * @brief Установить состояние блока интегрирования
     *
     * @param newState Новое значение состояния для установки
     */
    void setIntegratorState(const T& newState)
    {
        std::lock_guard<std::mutex> lock(mtx);
        integrator.setState(newState);
    }

    /**
     * @brief Получить ссылку на выходные данные блока
     *
     * @return Ссылка на выходные данные
     */
    const T& getOutput()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return pidOutput;
    }

    /**
     * @brief Обнулить текущее состояние блока
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);
        P = T{0};
        I = T{0};
        D = T{0};
        derivative.reset();
        integrator.reset();
        pidOutput = T{0};
    }
};
}
