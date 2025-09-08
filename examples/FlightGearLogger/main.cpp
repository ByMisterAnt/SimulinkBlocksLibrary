#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include <functional>
#include <OpenXLSX.hpp>
#include "AsyncExelWriter.h"

#include "../../include/SimulinkBlocksLibrary.hpp"
#include "../../include/Flightgear/net_ctrls.hxx"
#include "../../include/Flightgear/net_fdm.hxx"

using namespace SimulinkBlock;
using namespace OpenXLSX;

using ValueGetter = std::function<double()>;

struct Parameter {
    std::string name;
    ValueGetter getter;
};

int main()
{
    SimulinkBlock::FlightGearReceiver<FGNetCtrls> ctrls_receiver(5501);
    SimulinkBlock::FlightGearReceiver<FGNetFDM>   fdm_receiver  (5503);

    FGNetCtrls ctrls;
    FGNetFDM   fdm;

    double dt = 0.033;
    double cur_time = 0.0;

    std::vector<Parameter> parameters = {
        {"v_body_u", [&]() { return SimulinkBlock::L2B(fdm.v_body_u); }},
        {"v_body_v", [&]() { return SimulinkBlock::L2B(fdm.v_body_v); }},
        {"v_body_w", [&]() { return SimulinkBlock::L2B(fdm.v_body_w); }},
        {"vcas", [&]() { return SimulinkBlock::L2B(fdm.vcas); }},
        {"A_X_pilot", [&]() { return SimulinkBlock::L2B(fdm.A_X_pilot); }},
        {"A_Y_pilot", [&]() { return SimulinkBlock::L2B(fdm.A_Y_pilot); }},
        {"A_Z_pilot", [&]() { return SimulinkBlock::L2B(fdm.A_Z_pilot); }},
        {"alpha", [&]() { return SimulinkBlock::L2B(fdm.alpha); }},
        {"beta", [&]() { return SimulinkBlock::L2B(fdm.beta); }},
        {"phi", [&]() { return SimulinkBlock::L2B(fdm.phi); }},
        {"phidot", [&]() { return SimulinkBlock::L2B(fdm.phidot); }},
        {"theta", [&]() { return SimulinkBlock::L2B(fdm.theta); }},
        {"thetadot", [&]() { return SimulinkBlock::L2B(fdm.thetadot); }},
        {"psi", [&]() { return SimulinkBlock::L2B(fdm.psi); }},
        {"psidot", [&]() { return SimulinkBlock::L2B(fdm.psidot); }},
        {"altitude", [&]() { return SimulinkBlock::L2B(fdm.altitude); }},
        {"elevator", [&]() { return SimulinkBlock::L2B(ctrls.elevator); }},
        {"throttle", [&]() { return SimulinkBlock::L2B(ctrls.throttle[0]); }},
        {"aileron", [&]() { return SimulinkBlock::L2B(ctrls.aileron); }},
        {"rudder", [&]() { return SimulinkBlock::L2B(ctrls.rudder); }},
        {"cur_time", [&]() { return cur_time; }}
    };

    // Извлекаем имена для заголовков
    std::vector<std::string> headers;
    headers.reserve(parameters.size());
    for (const auto& p : parameters) {
        headers.push_back(p.name);
    }

    try {
        AsyncExcelWriter writer("flight_data.xlsx", headers);

        for (;;) {
            // Получаем свежие данные
            std::memcpy(&ctrls, &ctrls_receiver.getOutput(), sizeof(FGNetCtrls));
            std::memcpy(&fdm, &fdm_receiver.getOutput(), sizeof(FGNetFDM));

            std::vector<double> row_data;
            row_data.reserve(parameters.size());
            for (const auto& param : parameters) {
                row_data.push_back(param.getter());
            }

            writer.addToQueue(std::move(row_data));

            cur_time += dt;
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt * 1000)));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
