/*
 * Для работы с примером необходимо запустить Flightgear
 * c флагами:
 * --native-ctrls=socket,out,30,127.0.0.1,5501,udp
 * --native-ctrls=socket,in,30,127.0.0.1,5502,udp
 * --native-fdm=socket,out,30,127.0.0.1,5503,udp
 * где 30 - число раз в секунду отправки данных
 */

#include <iostream>
#include <thread>
#include <cstring>
#include <iostream>

#include "../../include/SimulinkBlocksLibrary.hpp"

// Структуры данных из репозитория FlightGear
#include "../../include/Flightgear/net_ctrls.hxx"
#include "../../include/Flightgear/net_fdm.hxx"


using namespace SimulinkBlock;

int main()
{
    // Класс для отправки управляющих параметров
    SimulinkBlock::SendUdp<FGNetCtrls> ctrls_sender("127.0.0.1", 5502);

    // Классы для получения данных
    SimulinkBlock::FlightGearReceiver<FGNetCtrls> ctrls_receiver(5501);
    SimulinkBlock::FlightGearReceiver<FGNetFDM>   fdm_receiver  (5503);

    // Структуры для работы с данными
    FGNetCtrls ctrls;
    FGNetFDM   fdm;

    double dt = 0.033; //!< Шаг рассчёта

    // ********************* Настройки контура бокового управления *********************

    SimulinkBlock::LateralControl<double> lateral;

    // Настройки коэффициентов контуров управления
    lateral.setAileronControllCoeffs(-0.1, -1.0, -0.1, -0.5, -0.01);
    lateral.setRudderControllCoeffs(-0.1, -0.1);

    // Настройки ограничений контуров
    lateral.setRollSaturationLimits(-0.3, 0.3);
    lateral.setRudderSaturationLimits(-0.3, 0.3);
    lateral.setAileronsSaturationLimits(-1.0, 1.0);

    // Включение/отключение контуров управления
    lateral.enableYawAngleControl(true);
    lateral.enableRollAngleControl(true);
    lateral.enableRudderControl(false);

    // *********************************************************************************

    // ********************* Настройки контура продольного управления *********************

    SimulinkBlock:LongitudalControl<double> longitudal;

    // Настройки коэффициентов контуров управления
    longitudal.setAltitudePidCoeffs(1.5, 0.5, 0.1);
    longitudal.setAngularVelocityPidCoeffs(-1.0, -0.01, -0.01);
    longitudal.setPitchAnglePidCoeffs(1.0, 0.0, 0.0);
    longitudal.setVelocityPidCoeffs(1.0, 0.01, 0);

    // Настройки ограничений контуров (ограничение на угол тангажа)
    longitudal.setSaturationLimits(-0.3, 0.3);

    // Включение/отключение контуров управления
    longitudal.enableAltitudeControl(true);
    longitudal.enableAngularVelocityControl(true);
    longitudal.enablePitchAngleControl(true);

    // *********************************************************************************

    for (;;)
    {
        // Сохранение "правильного" заполнения структуры, управляющих параметров
        std::memcpy(&ctrls, &ctrls_receiver.getOutput(), sizeof(FGNetCtrls));

        // Получение данных о параметрах математической модели
        std::memcpy(&fdm, &fdm_receiver.getOutput(), sizeof(FGNetFDM));

        // ********************* Логика работы бокового контура управления *********************

        double desiredPsi = 0/*2*/; // Желаемый угол курса (0 - 6.24)

        /*if (std::abs(L2B(fdm.psi) - desiredPsi) > 0.9) {
            lateral.enableYawAngleControl(true);
        } else*/ {
            lateral.enableYawAngleControl(false);
        }

        lateral.step(desiredPsi,
                     L2B(fdm.psi),
                     L2B(fdm.psidot),
                     L2B(fdm.phi),
                     L2B(fdm.phidot),
                     dt);

        auto latOut = lateral.getOutput();

        ctrls.rudder  = B2L(latOut.second);
        ctrls.aileron = B2L(latOut.first);

        // *********************************************************************************

        // ********************* Логика работы продольного контура управления *********************

        double desiredSpeed = 96; // Желаемая скорость
        double desiredAltitude = 200; // Желаемая высота

        /*if ( std::abs(L2B(fdm.altitude) - desiredAltitude) < 10 ) {
            longitudal.enableAltitudeControl(false);
        } else*/ {
            longitudal.enableAltitudeControl(true);
        }

        longitudal.step(desiredAltitude,
                        desiredSpeed,
                        L2B(fdm.altitude),
                        L2B(fdm.v_body_u),
                        L2B(fdm.theta),
                        L2B(fdm.thetadot),
                        dt);

        auto lonOut = longitudal.getOutput();

        ctrls.elevator    = B2L(lonOut.first);
        // ctrls.throttle[0] = B2L(lonOut.second);

        // *********************************************************************************


        // Отправка управляющих параметров
        ctrls_sender.send(ctrls);

        // Вывод некоторых параметров, для отладки
        std::cout << "elevator:  "    << SimulinkBlock::L2B(ctrls.elevator)    << '\t'
                  << "throttle[0]: "  << SimulinkBlock::L2B(ctrls.throttle[0]) << '\t'
                  << "altitude:  "    << SimulinkBlock::L2B(fdm.altitude)      << '\t'
                  << "v_body_u: "     << SimulinkBlock::L2B(fdm.v_body_u)      << '\t'
                  << "theta: "        << SimulinkBlock::L2B(fdm.theta)         << '\t'
                  << "aileron:  "     << SimulinkBlock::L2B(ctrls.aileron)     << '\t'
                  << "rudder: "       << SimulinkBlock::L2B(ctrls.rudder)      << '\t'
                  << "roll: "         << SimulinkBlock::L2B(fdm.phi)           << '\t'
                  << "yaw: "          << SimulinkBlock::L2B(fdm.psi)           << '\n';

        // Пауза, чтобы не спамить данными
        std::this_thread::sleep_for( std::chrono::milliseconds( uint(dt * 100) ) );
    }

    return 0;
}
