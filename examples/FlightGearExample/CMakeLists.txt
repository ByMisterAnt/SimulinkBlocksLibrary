cmake_minimum_required(VERSION 3.5)

project(FlightGearExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(FlightGearExample main.cpp)

include(GNUInstallDirs)
install(TARGETS FlightGearExample
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(FlightGearExample PUBLIC Boost::system)
