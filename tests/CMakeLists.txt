cmake_minimum_required(VERSION 3.5)

project(SimulinkLibraryTests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ($ENV{GOOGLETEST_DIR})
    get_filename_component(_GTEST_BASE $ENV{GOOGLETEST_DIR} REALPATH)
elseif (EXISTS "../../thirdparty/googletest/googletest/src")
    get_filename_component(_GTEST_BASE "../../thirdparty/googletest/googletest/src" REALPATH)
endif()

if (NOT GTEST_ROOT)
    if (_GTEST_BASE)
        message("Setting GTEST_ROOT to ${_GTEST_BASE}")
        set(GTEST_ROOT ${_GTEST_BASE})
    else()
        message("No GTEST_ROOT specified - using system defaults.")
    endif()
endif()

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
    message (FATAL_ERROR "No GTest Found")
endif()

add_executable(SimulinkLibraryTests main.cpp
    tst_derivative.cpp
    tst_integrator.cpp
    tst_lookuptable1d.cpp
    tst_randomnumbergenerator.cpp
    tst_ratelimiter.cpp
    tst_sinewavegenerator.cpp
    tst_triggeredsystem.cpp
    tst_whitenoize.cpp
    tst_saturation.cpp
    tst_pid.cpp
)

add_test(NAME SimulinkLibraryTests COMMAND SimulinkLibraryTests)

target_link_libraries(SimulinkLibraryTests PRIVATE GTest::GTest)
if (GMock_FOUND)
    target_link_libraries(SimulinkLibraryTests INTERFACE GTest::GMock)
endif()
