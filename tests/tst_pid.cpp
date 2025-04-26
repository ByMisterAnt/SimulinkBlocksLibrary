#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/PID.hpp"

using namespace testing;
using namespace SimulinkBlock;


// Класс теста для класса PID
class PIDTest : public ::testing::Test
{
protected:
    PID<double> pid_r{};
    PID<double> pid_saturated{1, 1, 1};

    // Настройка блока
    void SetUp() override
    {
        pid_r.setCoeffs(1, 1, 1);
        pid_saturated.setLimits(-0.5, 0.5, -0.5, 0.5);
    }
};

// Проверка инициализации состояния
TEST_F(PIDTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(pid_r.getOutput(), 0.0);
    //EXPECT_DOUBLE_EQ(pid_saturated.getOutput(), 0.0);
}

// Шаг с положительным значением
TEST_F(PIDTest, PositiveStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    pid_r.step(1, 0.1);
    EXPECT_DOUBLE_EQ(pid_r.getOutput(), 11.1);
    pid_r.reset();
}

// Шаг с положительным значением
TEST_F(PIDTest, NegativeStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    pid_r.step(-1, 0.1);
    EXPECT_DOUBLE_EQ(pid_r.getOutput(), -11.1);
    pid_r.reset();
}

// Шаг с положительным значением
TEST_F(PIDTest, PositiveSaturatedStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    pid_saturated.step(100, 0.1);
    EXPECT_DOUBLE_EQ(pid_saturated.getOutput(), 101);
    pid_r.reset();
}

// Шаг с положительным значением
TEST_F(PIDTest, NegativeSaturatedStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    pid_saturated.step(-100, 0.1);
    EXPECT_DOUBLE_EQ(pid_saturated.getOutput(), -101);
    pid_r.reset();
}
