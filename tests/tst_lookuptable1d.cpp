#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/LookupTable1D.hpp"

using namespace testing;
using namespace SimulinkBlock;

// Класс теста для класса LookupTable1D
class LookupTable1DTest : public ::testing::Test
{
protected:
    std::array<double, 5> inputs = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::array<double, 5> outputs = {10.0, 20.0, 30.0, 40.0, 50.0};
    LookupTable1D<double, 5> table{inputs, outputs};
};

// Проверка инициализации состояния
TEST_F(LookupTable1DTest, DefaultState)
{
    EXPECT_DOUBLE_EQ(table.getOutput(), 0.0);
}

// Тест для значения внутри таблицы
TEST_F(LookupTable1DTest, Interpolation)
{
    table.interpolate(2.5);
    EXPECT_DOUBLE_EQ(table.getOutput(), 25.0);
}

TEST_F(LookupTable1DTest, DownInterpolation)
{
    // Тест для значения ниже таблицы
    table.interpolate(0.5);
    EXPECT_DOUBLE_EQ(table.getOutput(), 5.0);
}

    // Тест для значения выше таблицы
TEST_F(LookupTable1DTest, UpInterpolation)
{
    table.interpolate(6.0);
    EXPECT_DOUBLE_EQ(table.getOutput(), 60.0);
}

// Тестирование метода extrapolate
// Тест для значения ниже таблицы
TEST_F(LookupTable1DTest, DownExtrapolation)
{
    table.interpolate(0.5);
    EXPECT_DOUBLE_EQ(table.getOutput(), 5.0);
}

// Тест для значения выше таблицы
TEST_F(LookupTable1DTest, UpExtrapolation)
{
    table.interpolate(6.0);
    EXPECT_DOUBLE_EQ(table.getOutput(), 60.0);
}

// Обнуление состояния блока
TEST_F(LookupTable1DTest, ResetState)
{
    table.reset();
    EXPECT_EQ(table.getOutput(), 0.0);
}
