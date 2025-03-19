#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <array>
#include <condition_variable>

#include <boost/asio.hpp>


namespace SimulinkBlock
{
using namespace boost::asio::ip;

// Receive net_ctrl Packet from FlightGear
/**
 * @brief Класс, реализующий логику работы блока
 * приёма пакета net_ctrl или net_fdm из FlightGear
 * @tparam T Тип получаемых данных
 *
 * @details Для работы класса необходимо запускать FlightGear
 * с флагом
 * --native-ctrls=socket,out,1,127.0.0.1,5501,udp
 * или
 * --native-fdm=socket,out,1,127.0.0.1,5501,udp
 * где:
 * 127.0.0.1 - IP адрес, на который отправляются данные;
 * 5501      - порт, на который отправляются данные;
 * 1         - сколько раз в секунду отправляются данные.
 */
template<typename T>
class FlightGearReceiver
{
public:
    /**
     * @brief Конструктор для инициализации блока с заданным портом по умолчанию
     *
     * @param port_ порт, на который принимать пакеты
     */
    FlightGearReceiver(int port_):
        socket_(ioContext, udp::endpoint(udp::v4(), port_))
    {
        reset();
        start();
    }

    ~FlightGearReceiver()
    {
        stop();
    }

    /**
     * @brief Ссылка на текущее состояние блока
     *
     * @return Указатель на структуру данных содержимого
     * получаемого пакета net_ctrl
     */
    const T& getOutput()
    {
        std::unique_lock<std::mutex> lock(dataMutex);
        condVar.wait(lock, [this]{ return initialized; });

        return output;
    }

    /**
     * @brief Обнулить текущий выход блока
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        std::memset(&output, 0, sizeof(T));
    }

    /**
     * @brief Запуск отдельного потока чтения данных
     */
    void start()
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        thread_ = std::thread([this] { ioContext.run(); });
        receiveData();
    }

    /**
     * @brief Остановка отдельного потока чтения данных
     */
    void stop()
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        ioContext.stop();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    std::thread thread_;
    boost::asio::io_context ioContext;
    udp::socket             socket_;
    udp::endpoint           remoteEndpoint;

    /*
     * Буфер полученных данных фиксированного размера
     *  (744 байта для 27 версии протокола)
     */
    std::array<char, sizeof(T)> recvBuffer;

    T output; //!< Структура, содержащая входные и выходные данные

    bool initialized = false;
    std::mutex dataMutex;
    std::condition_variable condVar;

    /**
     * @brief Функция обновления выходной структуры
     *
     * @param data Полученный массив байт
     */
    void update_data(const std::array<char, sizeof(T)> &data)
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        std::memcpy( &output, &data, sizeof(T) );
    }

    /**
     * @brief Функция чтения данных приходящих по udp
     */
    void receiveData()
    {
        socket_.async_receive_from(
            boost::asio::buffer(recvBuffer),
            remoteEndpoint,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred)
            {
                if (!error)
                {
                    update_data( recvBuffer );
                    initialized = true;
                    condVar.notify_one();
                }
                else
                {
                    std::cerr << "Ошибка получения UDP пакета: " << error.message() << std::endl;
                }

                receiveData();
            });
    }
};
}
