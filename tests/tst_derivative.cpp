#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/DerivativeBlock.hpp"

using namespace testing;
using namespace SimulinkBlock;


// Класс теста для класса DerivativeBlock
class DerivativeBlockTest : public ::testing::Test
{
protected:
    DerivativeBlock<double> derivative;
    DerivativeBlock<double> derivative_bounds{-10.0, 10.0};

    // Настройка блока
    void SetUp() override
    {
        derivative.setState(0.0);
    }
};

// Проверка инициализации состояния
TEST_F(DerivativeBlockTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(derivative.getOutput(), 0.0);
}

// Установка нового состояния
TEST_F(DerivativeBlockTest, SetState)
{
    derivative.setState(5.0);
    EXPECT_DOUBLE_EQ(derivative.getState(), 5.0);
}

// Шаг интегрирования с положительным значением
TEST_F(DerivativeBlockTest, DerivativePositiveStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    derivative.setState(0.0);
    derivative.step(2.5, 0.1);
    EXPECT_DOUBLE_EQ(derivative.getOutput(), 25);
}

// Шаг интегрирования с отрицательным значением
TEST_F(DerivativeBlockTest, DerivativeNegativeStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    derivative.setState(0.0);
    derivative.step(-1.0, 0.2);
    EXPECT_DOUBLE_EQ(derivative.getOutput(), -5);
}

// Интегрирование с превышением верхнего предела
TEST_F(DerivativeBlockTest, DerivativeUpLimitStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    derivative_bounds.setState(0.0);
    derivative_bounds.step(50, 0.5);
    EXPECT_EQ(derivative_bounds.getOutput(), 10);
}

// Интегрирование с превышением нижнего предела
TEST_F(DerivativeBlockTest, DerivativeDownLimitStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    derivative_bounds.setState(0.0);
    derivative_bounds.step(-50, 1.0);
    EXPECT_EQ(derivative_bounds.getOutput(), -10);
}

// Установка новых пределов интегрирования
TEST_F(DerivativeBlockTest, DerivativeNewLimitsStep)
{
    // т.к. сохраняет состояние обнуляем перед step
    derivative_bounds.setState(0.0);
    derivative_bounds.setLimits(-5.0, 5.0);
    derivative_bounds.step(10, 1.0);
    EXPECT_EQ(derivative_bounds.getOutput(), 5);
}

// Обнуление состояния блока
TEST_F(DerivativeBlockTest, DerivativeResetState)
{
    derivative.reset();
    EXPECT_EQ(derivative.getOutput(), 0.0);
}
