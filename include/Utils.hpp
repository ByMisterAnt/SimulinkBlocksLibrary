#pragma once

#include <cstddef>

namespace SimulinkBlock {

template <typename T>
T L2B(T big_endian_value)
{
    // Преобразование T из Big-Endian в Little-Endian
    T little_endian_value;
    char* big_endian_bytes = reinterpret_cast<char*>(&big_endian_value);
    char* little_endian_bytes = reinterpret_cast<char*>(&little_endian_value);

    // Меняем порядок байтов
    for (size_t i = 0; i < sizeof(T); ++i)
    {
        little_endian_bytes[i] = big_endian_bytes[sizeof(T) - 1 - i];
    }

    return little_endian_value;
}

template <typename T>
T B2L(T little_endian_value)
{
    // Преобразование T из Big-Endian в Little-Endian
    T big_endian_value;
    char* little_endian_bytes = reinterpret_cast<char*>(&little_endian_value);
    char* big_endian_bytes = reinterpret_cast<char*>(&big_endian_value);

    // Меняем порядок байтов
    for (size_t i = 0; i < sizeof(T); ++i)
    {
        big_endian_bytes[i] = little_endian_bytes[sizeof(T) - 1 - i];
    }

    return big_endian_value;
}
}
