

cmake_minimum_required(VERSION 3.12 )

include(pico_sdk_import.cmake)


 
message("Using RMP_PATH from environment ('$ENV{RMPLIB_PATH}')")


project(IRBlaster_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()



add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )



add_executable(IRBlaster IRBlaster.cpp)

pico_enable_stdio_usb(IRBlaster 0)
pico_enable_stdio_uart(IRBlaster 1)

add_subdirectory(rmpLib)

target_include_directories(IRBlaster PUBLIC rmpLib)
target_link_directories(IRBlaster PRIVATE rmpLib)

target_link_libraries(IRBlaster PRIVATE pico_stdlib hardware_pio hardware_pwm hardware_i2c rmpLib)


pico_add_extra_outputs(IRBlaster)









