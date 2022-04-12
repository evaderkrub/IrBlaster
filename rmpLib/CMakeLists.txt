
cmake_minimum_required(VERSION 3.12 )

include(pico_sdk_import.cmake)

project(rmpLib  C CXX ASM)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(rmpLib 
    rpPIO.cpp 
    rpFourSegDisplay.cpp 
    rpLED.cpp 
    rpIRComm.cpp 
    rpButton.cpp 
    rpGPIO.cpp
    )


target_link_libraries(rmpLib PUBLIC pico_stdlib hardware_pio hardware_pwm hardware_i2c)


