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


int main()
{
    // Класс для отправки управляющих параметров
    SimulinkBlock::SendUdp<FGNetCtrls>            ctrls_sender  ("127.0.0.1", 5502);

    // Классы для получения данных
    SimulinkBlock::FlightGearReceiver<FGNetCtrls> ctrls_receiver(5501);
    SimulinkBlock::FlightGearReceiver<FGNetFDM>   fdm_receiver  (5503);

    // Структуры для работы с данными
    FGNetCtrls ctrls;
    FGNetFDM   fdm;

    // Сохранение "правильного" заполнения структуры, управляющих параметров
    std::memcpy(&ctrls, &ctrls_receiver.getOutput(), sizeof(FGNetCtrls));

    for (;;)
    {
        // Получение данных о параметрах математической модели
        std::memcpy(&fdm, &fdm_receiver.getOutput(), sizeof(FGNetFDM));

        // Для корректного отображения чисел необходима функция L2B
        if ( SimulinkBlock::L2B(fdm.v_body_u) >  70 )
            ctrls.elevator = SimulinkBlock::L2B( double(-0.5) );

        ctrls.rudder = SimulinkBlock::L2B( double(0.1) );

        // Отправка управляющих параметров
        ctrls_sender.send(ctrls);

        std::cout << "elevator:  " << SimulinkBlock::L2B(ctrls.elevator) << " v_body_u: " << SimulinkBlock::L2B(fdm.v_body_u) << '\n';

        // Пауза, чтобы не спамить данными
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    return 0;
}
