cmake_minimum_required(VERSION 3.11)
project(clsp_libusb)

# write executables to bin directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable clang-tidy during the build phase
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

set(CLSP_LIBUSB_SOURCES
    src/main.cpp
    src/clsp.cpp
    src/clsp.hpp
)

add_executable(clsp_libusb ${CLSP_LIBUSB_SOURCES})

target_link_libraries(clsp_libusb usb-1.0)
