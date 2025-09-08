cmake_minimum_required(VERSION 3.5)

project(FlightGearLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти Boost
find_package(Boost REQUIRED COMPONENTS system)

# Подключить OpenXLSX как подмодуль
add_subdirectory(external/OpenXLSX)

# Создать исполняемый файл
add_executable(FlightGearLogger main.cpp
    AsyncExelWriter.h
)

# Линковка
target_link_libraries(FlightGearLogger
    PUBLIC
        Boost::system
        OpenXLSX::OpenXLSX
)

# Установка
include(GNUInstallDirs)
install(TARGETS FlightGearLogger
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
