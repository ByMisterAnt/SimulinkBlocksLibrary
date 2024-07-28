#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/TriggeredSubsystem.hpp"

using namespace testing;
using namespace SimulinkBlock;


// Класс теста для класса TriggeredSubsystem
class TriggeredSubsystemTest : public ::testing::Test
{
protected:
    TriggeredSubsystem<double, bool> subsystem;
};


// Проверка, что начальный выход равен нулю
TEST_F(TriggeredSubsystemTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(subsystem.getOutput(), 0.0);
}

// Проверка функции step с trigger_input = 1 и input = 5
TEST_F(TriggeredSubsystemTest, DefaultStep)
{
    subsystem.step(5, 1);
    EXPECT_EQ(subsystem.getOutput(), 5);
}

// Проверка функции reset,
// выход должен быть сброшен на ноль
TEST_F(TriggeredSubsystemTest, Reset)
{
    subsystem.reset();
    EXPECT_EQ(subsystem.getOutput(), 0);
}

// Проверка функции step со сменой триггера
// Выход должен остаться неизменным
TEST_F(TriggeredSubsystemTest, StepWithTrigChange)
{
    subsystem.step(5,  1);
    subsystem.step(10, 0);
    EXPECT_EQ(subsystem.getOutput(), 5); // Выход должен остаться неизменным
}

// Проверка step с trigger_input = 1 и input = -3
TEST_F(TriggeredSubsystemTest, StepNegativeInput)
{
    subsystem.reset();
    subsystem.step(-3, 1);
    EXPECT_EQ(subsystem.getOutput(), -3); // Выход должен быть установлен в -3
}

// Проверка reset после нескольких шагов
// Выход должен быть сброшен на ноль после сброса
TEST_F(TriggeredSubsystemTest, ResetAfterSeveralSteps)
{
    subsystem.step(10, 1);
    subsystem.step(20, 0);
    subsystem.step(15, 1);
    subsystem.reset();
    EXPECT_EQ(subsystem.getOutput(), 0);
}
