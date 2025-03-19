cmake_minimum_required(VERSION 3.5)

project(LibraryExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(LibraryExample main.cpp)

include(GNUInstallDirs)
install(TARGETS LibraryExample
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
