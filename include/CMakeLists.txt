# Interface library for header-only library
add_library(SimulinkBlocksLibrary INTERFACE)

# Specify include directories
target_include_directories(SimulinkBlocksLibrary INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
