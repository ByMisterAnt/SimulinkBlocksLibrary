#pragma once

#include <cstring>
#include <boost/asio.hpp>

namespace SimulinkBlock
{
/**
     * @brief Класс для отправки данных по протоколу UDP
     * @tparam T Тип данных для отправки
     */
template<typename T>
class SendUdp
{
public:
    /**
         * @brief Конструктор для DataSender
         * @param ip IP-адрес, на который будут отправлены данные
         * @param port Номер порта, на который будут отправлены данные
         */
    SendUdp(const std::string &ip = "127.0.0.1", unsigned short port = 5502) :
        socket(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)),
        server_endpoint(boost::asio::ip::address::from_string(ip), port)
    {
    }

    /**
         * @brief Отправить данные
         * @param data Данные для отправки
         */
    void send(const T &data)
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);

        std::array<char, sizeof(T)> buffer;
        std::memcpy(&buffer, &data, sizeof(T));
        socket.send_to(boost::asio::buffer(buffer), server_endpoint);
    }

private:
    boost::asio::io_service io_context; //!< Служба ввода-вывода для boost::asio
    boost::asio::ip::udp::socket socket; //!< UDP-сокет для отправки данных
    boost::asio::ip::udp::endpoint server_endpoint; //!< Конечная точка сервера для отправки данных
};
}
