cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(picolini)

set(CMAKE_C_STANDARD 11)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(picolini
	main.c ../ssd1306.c
)

target_include_directories(picolini
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../
)

target_link_libraries(picolini pico_stdlib hardware_i2c hardware_adc)

pico_enable_stdio_usb(picolini 1) 
pico_enable_stdio_uart(picolini 0) 

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(picolini)

