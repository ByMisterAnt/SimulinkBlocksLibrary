cmake_minimum_required(VERSION 3.5)

project(FlightGearExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти Boost
find_package(Boost REQUIRED COMPONENTS system)

# Подключить OpenXLSX как подмодуль
add_subdirectory(external/OpenXLSX)

# Создать исполняемый файл
add_executable(FlightGearExample main.cpp
    AsyncExelWriter.h
)

# Линковка
target_link_libraries(FlightGearExample
    PUBLIC
        Boost::system
        OpenXLSX::OpenXLSX
)

# Установка
include(GNUInstallDirs)
install(TARGETS FlightGearExample
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
