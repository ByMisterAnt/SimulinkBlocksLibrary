#pragma once

#include "../IntegratorBlock.hpp"
#include "../SaturationBlock.hpp"
#include <iostream>

namespace SimulinkBlock
{
/**
 * @brief Класс для реализации бокового канала управления с одним методом step
 */
template <typename T>
class LateralControl
{
private:
    std::mutex mtx; //!< Мьютекс для блокировки одновременного доступа к переменным класса

    std::pair<T, T> output; //!< Результат расчёта отклонение элеронов и руля направления

    // Канал руля направления
    T K_omega_yaw_rudder;
    T K_roll_rudder;

    bool rudderControlEnabled = true;

    // Канал элеронов
    IntegratorBlock<T> integrator_roll_aileron;
    IntegratorBlock<T> integrator_yaw_aileron;

    T K_omega_roll_aileron;
    T K_psi_aileron;
    T K_psi_i_aileron;
    T K_roll_aileron;
    T K_i_roll_aileron;

    SaturationBlock<T> desiredRollSaturation; //!< Ограничения желаемого угла крена
    SaturationBlock<T> aileronSaturation;     //!< Ограничения на отклонения элеронов
    SaturationBlock<T> rudderSaturation;      //!< Ограничения на отклонения руля направления

    bool yawAngleControlEnabled       = true; //!<
    bool rollAngleControlEnabled      = true; //!< Флаг для включения/выключения контура угла крена
    bool angularVelocityRollEnabled   = true; //!< Флаг для включения/выключения контура угловой скорости крена

public:
    /**
     * @brief Конструктор
     */
    LateralControl() {
        integrator_roll_aileron.setLimits(-1, 1);
    }

    /**
     * @brief Установка коэффициентов ограничения угла крена
     */
    void setRudderControllCoeffs(const T& K_omega_yaw_rudder, const T& K_roll_rudder)
    {
        std::lock_guard<std::mutex> lock(mtx);

        this->K_roll_rudder      = K_roll_rudder;
        this->K_omega_yaw_rudder = K_omega_yaw_rudder;
    }

    /**
     * @brief Установка коэффициентов ограничения угла крена
     */
    void setAileronControllCoeffs(const T& K_omega_roll_aileron,
                                  const T& K_roll_aileron,
                                  const T& K_i_roll_aileron,
                                  const T& K_psi_aileron,
                                  const T& K_psi_i_aileron)
    {
        std::lock_guard<std::mutex> lock(mtx);

        this->K_omega_roll_aileron = K_omega_roll_aileron;
        this->K_roll_aileron       = K_roll_aileron;
        this->K_i_roll_aileron     = K_i_roll_aileron;
        this->K_psi_aileron        = K_psi_aileron;
        this->K_psi_i_aileron      = K_psi_i_aileron;
    }

    /**
     * @brief Установка коэффициентов ограничения угла крена
     */
    void setRollSaturationLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        desiredRollSaturation.setLimits(min, max);
    }

    /**
     * @brief Установка коэффициентов ограничения угла крена
     */
    void setRudderSaturationLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        rudderSaturation.setLimits(min, max);
    }

    /**
     * @brief Установка коэффициентов ограничения угла крена
     */
    void setAileronsSaturationLimits(const T& min, const T& max)
    {
        std::lock_guard<std::mutex> lock(mtx);
        aileronSaturation.setLimits(min, max);
    }

    /**
     * @brief Включение/выключение контура руля направления
     */
    void enableRudderControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        rudderControlEnabled = enable;
    }

    /**
     * @brief Включение/выключение контура руля направления
     */
    void enableYawAngleControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        yawAngleControlEnabled = enable;
    }

    /**
     * @brief Включение/выключение контура руля направления
     */
    void enableRollAngleControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        rollAngleControlEnabled = enable;
    }

    /**
     * @brief Включение/выключение контура руля направления
     */
    void enableAngularVelocityRollControl(bool enable)
    {
        std::lock_guard<std::mutex> lock(mtx);
        angularVelocityRollEnabled = enable;
    }

    /**
     * @brief Основной метод step для последовательного вычисления всех контуров
     * @param desiredYawAngle желаемый угол курса
     * @param currentYawAngle текущий угол курса
     * @param desiredRollAngle желаемый угол крена (может быть рассчитан из курса)
     * @param currentRollAngle текущий угол крена
     * @param currentRollAngularVelocity текущая угловая скорость крена
     * @param dt шаг моделирования
     */
    void step(
        const T& desiredYawAngle,
        const T& currentYawAngle,
        const T& currentYawAngleVelocity,
        const T& currentRollAngle,
        const T& currentRollAngularVelocity,
        double dt)
    {
        std::lock_guard<std::mutex> lock(mtx);

        T inputForRollAngle = desiredYawAngle; // Вход для контура угла крена
        T inputForAngularVelocity = inputForRollAngle; // Вход для контура угловой скорости крена

        // ******************* Контур руля направления *******************

        output.second = T{0};

        if (rudderControlEnabled) {
            auto rudder = K_omega_yaw_rudder * currentYawAngleVelocity +
                          K_roll_rudder      * currentYawAngle;
            rudderSaturation.step(rudder);
            output.second = rudderSaturation.getOutput();
        }

        // ******************* Контур элеронов *******************
        // Контур угла курса
        if (yawAngleControlEnabled) {
            T yawError = currentYawAngle - desiredYawAngle;

            integrator_yaw_aileron.step(K_psi_i_aileron * yawError, dt);
            desiredRollSaturation.step( K_psi_aileron   * yawError +
                                        integrator_yaw_aileron.getOutput() );

            inputForRollAngle = desiredRollSaturation.getOutput();
            output.first = inputForRollAngle;
        }

        // Контур угла курса
        if (rollAngleControlEnabled) {
            desiredRollSaturation.step(inputForRollAngle);

            T rollError = currentRollAngle - desiredRollSaturation.getOutput();

            integrator_roll_aileron.step(rollError * K_i_roll_aileron, dt);

            inputForAngularVelocity = integrator_roll_aileron.getOutput() +
                                      K_roll_aileron * currentRollAngle;

            output.first = inputForAngularVelocity;
        }

        // Демпфер крена
        if (angularVelocityRollEnabled) {
            output.first = K_omega_roll_aileron * currentRollAngularVelocity +
                           inputForAngularVelocity;
            return;
        }

        output.first = desiredYawAngle;
    }

    /**
     * @brief Получить результат рассчёта
     * @return Результат рассчёта (например, руль направления и руль высоты или моменты)
     */
    const std::pair<T, T>& getOutput()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return output;
    }

    /**
     * @brief Обнулить текущее состояние интеграторов
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mtx);

        output = std::pair<T, T>{0};

        // Канал руля направления
        K_omega_yaw_rudder = T{0};
        K_roll_rudder = T{0};

        // Канал элеронов
        integrator_roll_aileron.reset();

        K_omega_roll_aileron = T{0};
        K_psi_aileron = T{0};
        K_roll_aileron = T{0};
        K_i_roll_aileron = T{0};
    }
};
}
