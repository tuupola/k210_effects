set(BUILDING_SDK "yes" CACHE INTERNAL "")

cmake_minimum_required(VERSION 3.0)
include(${SDK}/cmake/common.cmake)
project(firmware C CXX ASM)

include(${SDK}/cmake/macros.internal.cmake)

header_directories(${SDK}/lib)
add_subdirectory(${SDK}/lib sdk)

add_source_files(*.c)
include(${SDK}/cmake/executable.cmake)

add_subdirectory(external/hagl_hal)
add_subdirectory(external/hagl)

target_link_libraries(firmware hagl hagl_hal)

target_compile_definitions(firmware PRIVATE
    HAGL_HAL_USE_DOUBLE_BUFFER
    #HAGL_HAL_USE_TRIPLE_BUFFER
    #HAGL_HAL_USE_DMA
    HAGL_HAL_DEBUG
    #MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_SWAP_XY|MIPI_DCS_ADDRESS_MODE_MIRROR_Y
    #MIPI_DISPLAY_WIDTH=320
    #MIPI_DISPLAY_HEIGHT=240
)