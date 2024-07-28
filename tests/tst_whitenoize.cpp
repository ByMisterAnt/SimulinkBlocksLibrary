#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/WhiteNoiseGenerator.hpp"


// Проверка выходных данных после инициализации
TEST(WhiteNoiseGenerator, DefaultConstructor)
{
    SimulinkBlock::WhiteNoiseGenerator<double> generator(5, 2);
    EXPECT_EQ(generator.getOutput(), 0);
}

// Проверка выходных данных после инициализации
TEST(WhiteNoiseGenerator, ResetFunction)
{
    SimulinkBlock::WhiteNoiseGenerator<double> generator(5, 2);
    generator.step();
    generator.reset();
    EXPECT_EQ(generator.getOutput(), 0);
}
