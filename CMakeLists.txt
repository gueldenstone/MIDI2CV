cmake_minimum_required(VERSION 3.16)

# toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32_gcc.cmake)

# alias for reference
set(TARGET_NAME MIDI2CV)
set(CHIP_NAME STM32G474xx)

# project
project(${TARGET_NAME} C CXX ASM)

# main executable with sources an includes
add_executable(${TARGET_NAME})
add_subdirectory(src)
target_include_directories(
  ${TARGET_NAME}
  PUBLIC
  include
)
target_compile_definitions(${TARGET_NAME} PUBLIC ${CHIP_NAME})

# set standard
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)

# CMSIS
stm32_fetch_cmsis(G4)
find_package(CMSIS COMPONENTS STM32G4 REQUIRED)
target_link_libraries(${TARGET_NAME} CMSIS::STM32::G474RE STM32::Nano)
stm32_print_size_of_target(${TARGET_NAME})

# etl
add_subdirectory(lib/etl)
target_link_libraries(${TARGET_NAME} etl)

# stmlibxx
add_subdirectory(lib/stmlibxx)
target_link_libraries(${TARGET_NAME} stmlibxx)

# get_target_property()
