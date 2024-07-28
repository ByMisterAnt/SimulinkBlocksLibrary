#include "../include/RateLimiter.hpp"
#include <gtest/gtest.h>

using namespace SimulinkBlock;

// Test fixture for RateLimiter class
class RateLimiterTest : public ::testing::Test
{
protected:
    RateLimiter<double> limiter{5, 3};
};

// Проверка инициализации состояния
TEST_F(RateLimiterTest, Initialization)
{
    EXPECT_EQ(limiter.getOutput(), 0);
}

// Установка нового состояния
TEST_F(RateLimiterTest, SetState)
{
    limiter.setState(10);
    EXPECT_EQ(limiter.getOutput(), 10);
}

// Обнуление состояния блока
TEST_F(RateLimiterTest, Reset)
{
    limiter.reset();
    EXPECT_EQ(limiter.getOutput(), 0);
}

// Шаг расчёта с превышением верхнего предела
TEST_F(RateLimiterTest, UpLimit)
{
    limiter.step(7, 1);
    EXPECT_EQ(limiter.getOutput(), 5);
}

// Шаг расчёта с превышением нижнего предела
TEST_F(RateLimiterTest, DownLimit)
{
    limiter.reset();
    limiter.step(2, 1);
    EXPECT_EQ(limiter.getOutput(), 3);
}
