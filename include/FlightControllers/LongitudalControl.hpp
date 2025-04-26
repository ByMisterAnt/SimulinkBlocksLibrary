#pragma once

#include "../PID.hpp"

namespace SimulinkBlock
{
/**
 * @brief Класс для реализации контура тангажа с одним методом step
 */
template <typename T>
class LongitudalControl
{
private:
    std::mutex mtx; //!< Мьютекс для блокировки одновременного доступа к переменным класса

    T output; //!< Результат рассчёта

    PID<T> altitudePid;        //!< ПИД-регулятор для контура высоты
    PID<T> pitchAnglePid;      //!< ПИД-регулятор для контура угла тангажа
    PID<T> angularVelocityPid; //!< ПИД-регулятор для контура угловой скорости

    bool altitudeControlEnabled        = true; //!< Флаг для включения/выключения контура высоты
    bool pitchAngleControlEnabled      = true; //!< Флаг для включения/выключения контура угла тангажа
    bool angularVelocityControlEnabled = true; //!< Флаг для включения/выключения контура угловой скорости


public:
    /**
     * @brief Конструктор для инициализации контура тангажа
     */
    LongitudalControl() = default;

    /**
     * @brief Установка коэффициентов для ПИД-регулятора высоты
     */
    void setAltitudePidCoeffs(const T& p, const T& i, const T& d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        altitudePid.setCoeffs(p, i, d);
    }

    /**
     * @brief Установка коэффициентов для ПИД-регулятора угла тангажа
     */
    void setPitchAnglePidCoeffs(const T& p, const T& i, const T& d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        pitchAnglePid.setCoeffs(p, i, d);
    }

    /**
     * @brief Установка коэффициентов для ПИД-регулятора угловой скорости
     */
    void setAngularVelocityPidCoeffs(const T& p, const T& i, const T& d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        angularVelocityPid.setCoeffs(p, i, d);
    }

    /**
     * @brief Включение/выключение контура высоты
     */
    void enableAltitudeControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        altitudeControlEnabled = enable;
    }

    /**
     * @brief Включение/выключение контура угла тангажа
     */
    void enablePitchAngleControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        pitchAngleControlEnabled = enable;
    }

    /**
     * @brief Включение/выключение контура угловой скорости
     */
    void enableAngularVelocityControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        angularVelocityControlEnabled = enable;
    }

    /**
     * @brief  Основной метод step для последовательного вычисления всех контуров
     * @param desiredAltitude желаемая высота
     * @param currentAltitude текущая высота
     * @param currentPitchAngle текущий угол тангажа
     * @param currentAngularVelocity текущая угловая скорость
     * @param dt шаг моделирования
     */
    void step(
        const T& desiredAltitude,
        const T& currentAltitude,
        const T& currentPitchAngle,
        const T& currentAngularVelocity,
        double dt)
    {
        std::lock_guard<std::mutex> lock(mtx);

        T inputForPitchAngle = desiredAltitude; // Вход для контура угла тангажа
        T inputForAngularVelocity = desiredAltitude; // Вход для контура угловой скорости

        // Контур высоты
        if (altitudeControlEnabled) {
            T altitudeError = desiredAltitude - currentAltitude;
            altitudePid.step(altitudeError, dt);
            inputForPitchAngle = altitudePid.getOutput();
            output = inputForPitchAngle;
        }

        // Контур угла тангажа
        if (pitchAngleControlEnabled) {
            T pitchAngleError = inputForPitchAngle - currentPitchAngle;
            pitchAnglePid.step(pitchAngleError, dt);
            inputForAngularVelocity = pitchAnglePid.getOutput();
            output = inputForAngularVelocity;
        }

        // Контур угловой скорости
        if (angularVelocityControlEnabled) {
            T angularVelocityError = inputForAngularVelocity - currentAngularVelocity;
            angularVelocityPid.step(angularVelocityError, dt);
            output = angularVelocityPid.getOutput();
            return;
        }

        output = desiredAltitude;
    }

    /**
     * @brief Получить результат рассчёта
     *
     * @return Результат рассчёта
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

        output = T(0);

        altitudeControlEnabled        = true;
        pitchAngleControlEnabled      = true;
        angularVelocityControlEnabled = true;

        altitudePid.reset();
        pitchAnglePid.reset();
        angularVelocityPid.reset();
    }
};
}
