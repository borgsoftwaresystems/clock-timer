cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project)
pico_sdk_init()

# add_executable(  clock_timer
#   clock_timer.cpp
# )

# pico_enable_stdio_usb(clock_timer 1)
# pico_enable_stdio_uart(clock_timer 0)

# pico_add_extra_outputs(clock_timer)

# target_link_libraries(clock_timer pico_stdlib hardware_rtc pico_display pico_explorer)


add_executable(pico_explorer_clock_timer
 pico_explorer_clock_timer.cpp  
 arc.cpp
 clock_timer_common.cpp
)

pico_enable_stdio_usb(pico_explorer_clock_timer 1)
pico_enable_stdio_uart(pico_explorer_clock_timer 0)

pico_add_extra_outputs(pico_explorer_clock_timer)

target_link_libraries(pico_explorer_clock_timer pico_stdlib hardware_rtc pico_display pico_explorer)


add_executable(pico_display_clock_timer
 pico_display_clock_timer.cpp  
 arc.cpp
 clock_timer_common.cpp
)

pico_enable_stdio_usb(pico_display_clock_timer 1)
pico_enable_stdio_uart(pico_display_clock_timer 0)

pico_add_extra_outputs(pico_display_clock_timer)

target_link_libraries(pico_display_clock_timer pico_stdlib hardware_rtc pico_display pico_explorer)


add_subdirectory(drivers)
add_subdirectory(libraries)