#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/SaturationBlock.hpp"

using namespace testing;
using namespace SimulinkBlock;


// Класс теста для класса SaturationBlock
class SaturationBlockTest : public ::testing::Test
{
protected:
    SaturationBlock<double> saturation{-10,10};
};

// Проверка инициализации состояния
TEST_F(SaturationBlockTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(saturation.getOutput(), 0.0);
}

// Шаг с положительным значением
TEST_F(SaturationBlockTest, PositiveStep)
{
    saturation.step(2.5);
    EXPECT_DOUBLE_EQ(saturation.getOutput(), 2.5);
}

// Шаг с отрицательным значением
TEST_F(SaturationBlockTest, NegativeStep)
{
    saturation.step(-1.0);
    EXPECT_DOUBLE_EQ(saturation.getOutput(), -1.0);
}

// Шаг с превышением верхнего предела
TEST_F(SaturationBlockTest, UpLimitStep)
{
    saturation.step(50);
    EXPECT_EQ(saturation.getOutput(), 10);
}

// Установка новых пределов
TEST_F(SaturationBlockTest, NewLimitsStep)
{
    saturation.setLimits(-12, 11);
    saturation.step(11);
    EXPECT_EQ(saturation.getOutput(), 11);
}

// Шаг с превышением нижнего предела
TEST_F(SaturationBlockTest, DownLimitStep)
{
    saturation.setLimits(-12, 11);
    saturation.step(-100);
    EXPECT_EQ(saturation.getOutput(), -12);
}


// Обнуление состояния блока
TEST_F(SaturationBlockTest, SaturationResetState)
{
    saturation.reset();
    EXPECT_EQ(saturation.getOutput(), 0.0);
}
