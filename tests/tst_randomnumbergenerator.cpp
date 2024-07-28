#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/RandomNumberGenerator.hpp"


// Проверка выходных данных после инициализации
TEST(RandomNumberGenerator, DefaultConstructor)
{
    SimulinkBlock::RandomNumberGenerator<double> generator;
    EXPECT_EQ(generator.getOutput(), 0);
}

// Проверка выходных данных после инициализации
TEST(RandomNumberGenerator, ResetFunction)
{
    SimulinkBlock::RandomNumberGenerator<double> generator;
    generator.step();
    generator.reset();
    EXPECT_EQ(generator.getOutput(), 0);
}
