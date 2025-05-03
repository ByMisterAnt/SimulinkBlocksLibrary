#pragma once

#include "../PID.hpp"
#include "../SaturationBlock.hpp"

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

    std::pair<T, T> output; //!< Результат рассчёта

    PID<T> velocityPid;        //!< ПИД-регулятор для контура скорости
    PID<T> altitudePid;        //!< ПИД-регулятор для контура высоты
    PID<T> pitchAnglePid;      //!< ПИД-регулятор для контура угла тангажа
    PID<T> angularVelocityPid; //!< ПИД-регулятор для контура угловой скорости


    SaturationBlock<T> desiredPitchSaturation; //!< Ограничения желаемого угла тангажа

    bool speedControlEnabled           = true; //!< Флаг для включения/выключения контура скорости
    bool altitudeControlEnabled        = true; //!< Флаг для включения/выключения контура высоты
    bool pitchAngleControlEnabled      = true; //!< Флаг для включения/выключения контура угла тангажа
    bool angularVelocityControlEnabled = true; //!< Флаг для включения/выключения контура угловой скорости


public:
    /**
     * @brief Конструктор для инициализации контура тангажа
     */
    LongitudalControl() {
        velocityPid.setLimits(-1.0, 1.0, -1.0, 1.0);
        altitudePid.setLimits(-1.0, 1.0, -1.0, 1.0);
        pitchAnglePid.setLimits(-1.0, 1.0, -1.0, 1.0);
        angularVelocityPid.setLimits(-1.0, 1.0, -1.0, 1.0);
    };

    /**
     * @brief Установка коэффициентов для ограничения угла тангажа
     */
    void setSaturationLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        desiredPitchSaturation.setLimits(min, max);
    }

    /**
     * @brief Установка коэффициентов для ПИД-регулятора скорости
     */
    void setVelocityPidCoeffs(const T& p, const T& i, const T& d)
    {
        std::lock_guard<std::mutex> lock(mtx);
        velocityPid.setCoeffs(p, i, d);
    }

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
    void enableSpeedControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        speedControlEnabled = enable;
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
     * @param currentVelocity текущая скорость
     * @param dt шаг моделирования
     */
    void step(
        const T& desiredAltitude,
        const T& desiredVelocity,
        const T& currentAltitude,
        const T& currentVelocity,
        const T& currentPitchAngle,
        const T& currentAngularVelocity,
        double dt)
    {
        std::lock_guard<std::mutex> lock(mtx);

        T inputForPitchAngle = desiredAltitude; // Вход для контура угла тангажа
        T inputForAngularVelocity = desiredAltitude; // Вход для контура угловой скорости

        // Контур скорости
        if (speedControlEnabled) {
            velocityPid.step(desiredVelocity - currentVelocity, dt);
            output.second = velocityPid.getOutput();
        } else {
            output.second = T{0};
        }

        // Контур высоты
        if (altitudeControlEnabled) {
            T altitudeError = desiredAltitude - currentAltitude;
            altitudePid.step(altitudeError, dt);
            inputForPitchAngle = altitudePid.getOutput();
            output.first = inputForPitchAngle;
        }

        // Контур угла тангажа
        if (pitchAngleControlEnabled) {
            // Ограничение на угол тангажа чтобы не было сваливания
            desiredPitchSaturation.step(inputForPitchAngle);
            inputForPitchAngle = desiredPitchSaturation.getOutput();

            T pitchAngleError = inputForPitchAngle - currentPitchAngle;
            pitchAnglePid.step(pitchAngleError, dt);
            inputForAngularVelocity = pitchAnglePid.getOutput();
            output.first = inputForAngularVelocity;
        }

        // Контур угловой скорости
        if (angularVelocityControlEnabled) {
            T angularVelocityError = inputForAngularVelocity - currentAngularVelocity;
            angularVelocityPid.step(angularVelocityError, dt);
            output.first = angularVelocityPid.getOutput();
            return;
        }

        output.first = desiredAltitude;
    }

    /**
     * @brief Получить результат рассчёта
     *
     * @return Результат рассчёта, отклонение РУС и РУД
     */
    const std::pair<T, T>& getOutput()
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

        output = std::pair<T, T>{0};

        altitudeControlEnabled        = true;
        pitchAngleControlEnabled      = true;
        angularVelocityControlEnabled = true;

        altitudePid.reset();
        pitchAnglePid.reset();
        angularVelocityPid.reset();
    }
};
}
