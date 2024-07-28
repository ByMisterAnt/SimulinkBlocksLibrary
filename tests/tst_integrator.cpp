#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/IntegratorBlock.hpp"

using namespace testing;
using namespace SimulinkBlock;


// Класс теста для класса IntegratorBlock
class IntegratorBlockTest : public ::testing::Test
{
protected:
    IntegratorBlock<double> integrator;
    IntegratorBlock<double> integrator_bounds{-10.0, 10.0, 0.0};

    // Настройка блока
    void SetUp() override
    {
        integrator.setState(0.0);
    }
};

// Проверка инициализации состояния
TEST_F(IntegratorBlockTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(integrator.getOutput(), 0.0);
}

// Установка нового состояния
TEST_F(IntegratorBlockTest, SetState)
{
    integrator.setState(5.0);
    EXPECT_DOUBLE_EQ(integrator.getOutput(), 5.0);
}

// Шаг интегрирования с положительным значением
TEST_F(IntegratorBlockTest, IntegrationPositiveStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    integrator.setState(0.0);
    integrator.step(2.5, 0.1);
    EXPECT_DOUBLE_EQ(integrator.getOutput(), 0.25);
}

// Шаг интегрирования с отрицательным значением
TEST_F(IntegratorBlockTest, IntegrationNegativeStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    integrator.setState(0.0);
    integrator.step(-1.0, 0.2);
    EXPECT_DOUBLE_EQ(integrator.getOutput(), -0.2);
}

// Интегрирование с превышением верхнего предела
TEST_F(IntegratorBlockTest, IntegrationUpLimitStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    integrator_bounds.setState(0.0);
    integrator_bounds.step(50, 0.5);
    EXPECT_EQ(integrator_bounds.getOutput(), 10);
}

// Интегрирование с превышением нижнего предела
TEST_F(IntegratorBlockTest, IntegrationDownLimitStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    integrator_bounds.setState(0.0);
    integrator_bounds.step(-10, 1.0);
    EXPECT_EQ(integrator_bounds.getOutput(), -10);
}

// Установка новых пределов интегрирования
TEST_F(IntegratorBlockTest, IntegrationNewLimitsStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    integrator_bounds.setState(0.0);
    integrator_bounds.setLimits(-5.0, 5.0);
    integrator_bounds.step(10, 1.0);
    EXPECT_EQ(integrator_bounds.getOutput(), 5);
}

// Обнуление состояния блока
TEST_F(IntegratorBlockTest, IntegratorResetState)
{
    integrator.reset();
    EXPECT_EQ(integrator.getOutput(), 0.0);
}
