#pragma once

#include <cmath>
#include <vector>
#include <random>

#include "IntegratorBlock.hpp"
#include "DerivativeBlock.hpp"
#include "LookupTable1D.hpp"
#include "TriggeredSubsystem.hpp"
#include "RandomNumberGenerator.hpp"
#include "WhiteNoiseGenerator.hpp"
#include "SaturationBlock.hpp"
#include "SineWaveGenerator.hpp"
#include "RateLimiter.hpp"

namespace SimulinkBlock
{
/**
 * @brief Функция, реализующая логику работы блока насыщения
 * @param input Входной сигнал
 * @param upperLimit Верхняя граница ограничения значения
 * @param lowerLimit Нижняя граница ограничения значения
 *
 * @return Сигнал с учётом ограничений
 */
template <typename T>
const T& saturation(const T& input,
                    const T& upperLimit,
                    const T& lowerLimit)
{
    if ( input > upperLimit)
        return upperLimit;
    else if ( input < lowerLimit)
        return lowerLimit;
    else
        return input;
}
}
