#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../include/SineWaveGenerator.hpp"


// Проверка выходных данных после инициализации
TEST(SineWaveGeneratorTest, DefaultConstructor)
{
    SimulinkBlock::SineWaveGenerator<double, double> generator;
    EXPECT_EQ(generator.getOutput(), 0.0);
}

// Проверка выходных данных для нулевого момента времени
TEST(SineWaveGeneratorTest, StepDefaultConstructor)
{
    SimulinkBlock::SineWaveGenerator<double, double> generator;
    generator.step(0.0);
    EXPECT_NEAR(generator.getOutput(), 0.0, 0.0001);
}

// Проверка выходных данных после настройки
TEST(SineWaveGeneratorTest, SetupWithSetup)
{
    SimulinkBlock::SineWaveGenerator<double, double> generator;
    generator.setup(2.0, 1.0, 0.0);
    generator.step(0.0);
    EXPECT_NEAR(generator.getOutput(), 0.0, 0.0001);
}

// Проверка выходных данных после сброса
TEST(SineWaveGeneratorTest, ResetFunction)
{
    SimulinkBlock::SineWaveGenerator<double, double> generator;
    generator.setup(2.0, 1.0, 0.0);
    generator.reset();
    generator.step(0.0);
    EXPECT_NEAR(generator.getOutput(), 0.0, 0.0001);
}
