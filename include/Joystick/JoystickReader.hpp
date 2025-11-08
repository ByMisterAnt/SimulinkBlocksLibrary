#pragma once

#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <condition_variable>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

namespace SimulinkBlock
{

/**
 * @brief Структура положения элементов управления джойстика
 */
struct JoystickState
{
    float x = 0.0f;        //!< тангаж [-1.0, 1.0]
    float y = 0.0f;        //!< крен [-1.0, 1.0]
    float rz = 0.0f;       //!< курс [-1.0, 1.0]
    float throttle = 0.0f; //!< тяга [0.0, 1.0]

    bool initialized = false;
};

/**
 * @brief Класс для получения данных с джойстика
 */
class JoystickReader
{
public:
    /**
     * @brief Инициализирует чтение с указанного устройства
     * @param devicePath путь к устройству
     */
    explicit JoystickReader(const std::string& devicePath)
        : devicePath_(devicePath)
    {
        reset();
        start();
    }

    ~JoystickReader()
    {
        stop();
    }

    /**
     * @brief Получить текущее состояние джойстика
     * Блокируется до получения первого валидного пакета.
     */
    const JoystickState& getOutput()
    {
        std::unique_lock<std::mutex> lock(dataMutex_);
        condVar_.wait(lock, [this] { return output_.initialized; });
        return output_;
    }

    /**
     * @brief Обнулить выходное состояние
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        output_ = JoystickState{};
    }

    /**
     * @brief Запустить фоновый поток опроса устройства
     */
    void start()
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (thread_.joinable()) return;

        fd_ = open(devicePath_.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd_ < 0) {
            std::cerr << "Error when trying open device: " << devicePath_ << std::endl;
            return;
        }

        thread_ = std::thread([this] {
            pollLoop();
        });
    }

    /**
     * @brief Остановить поток и закрыть устройство
     */
    void stop()
    {
        std::lock_guard<std::mutex> lock(threadMutex_);
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    std::string devicePath_;
    std::thread thread_;
    mutable std::mutex threadMutex_;

    int fd_ = -1;
    JoystickState output_;
    mutable std::mutex dataMutex_;
    mutable std::condition_variable condVar_;

    static constexpr int XY_MIN = 0;
    static constexpr int XY_MAX = 1023;
    static constexpr int RZ_MIN = 0;
    static constexpr int RZ_MAX = 255;
    static constexpr int THR_MIN = 0;
    static constexpr int THR_MAX = 255;

    float normalizeXY(int value) const
    {
        float centered = static_cast<float>(value - 512);
        return std::clamp(centered / 512.0f, -1.0f, 1.0f);
    }

    float normalizeRZ(int value) const
    {
        float centered = static_cast<float>(value - 128);
        return std::clamp(centered / 128.0f, -1.0f, 1.0f);
    }

    float normalizeThrottle(int value) const
    {
        return std::clamp(static_cast<float>(value - THR_MIN) / (THR_MAX - THR_MIN), 0.0f, 1.0f);
    }

    void pollLoop()
    {
        input_event ev;
        JoystickState localState;

        while (fd_ >= 0) {
            ssize_t n = read(fd_, &ev, sizeof(ev));
            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    continue;
                } else {
                    std::cerr << "Error joystick reading: " << strerror(errno) << std::endl;
                    break;
                }
            }

            if (n != sizeof(ev)) continue;
            if (ev.type != EV_ABS) continue;

            bool changed = false;

            switch (ev.code) {
            case ABS_X:
                localState.x = normalizeXY(ev.value);
                changed = true;
                break;
            case ABS_Y:
                localState.y = normalizeXY(ev.value);
                changed = true;
                break;
            case ABS_RZ:
                localState.rz = normalizeRZ(ev.value);
                changed = true;
                break;
            case ABS_THROTTLE:
                localState.throttle = normalizeThrottle(ev.value);
                changed = true;
                break;
            default:
                break;
            }

            if (changed) {
                std::lock_guard<std::mutex> lock(dataMutex_);
                output_ = localState;
                output_.initialized = true;
                condVar_.notify_all();
            }
        }
    }
};

}
