# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#
#   Set the language standards.
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

#
#   Add extra components.
#
list(APPEND EXTRA_COMPONENT_DIRS components/Tab5)
list(APPEND EXTRA_COMPONENT_DIRS components/ALU)

project(Tab5SSEM
        LANGUAGES CXX C
        VERSION 1.0.0
        DESCRIPTION "Small Scale Experimental Machine"
        HOMEPAGE_URL "https://github.com/markstevens/Tab5SSEM")

#
#   Suppress warnings from third-party managed components only.
#
idf_component_set_property(m5stack__m5gfx    COMPILE_OPTIONS "-w" APPEND)
idf_component_set_property(m5stack__m5unified COMPILE_OPTIONS "-w" APPEND)

#
#   Suppress spurious warnings from ESP-IDF internal headers.
#   -Wno-literal-suffix silences the invalid-suffix-on-literal warning
#   in esp_hw_support/periph_ctrl.h caused by a missing space between
#   a string literal and the CONFIG_IDF_TARGET macro in ESP-IDF v5.5.x.
#   The flag is only valid for C++/ObjC++, so guard it with a generator
#   expression to prevent it being passed to the C compiler.
#
#   -Wno-missing-field-initializers silences the warning fired by the
#   gpio_ll_matrix_out_default() inline function in the ESP32-P4 GPIO
#   HAL header (gpio_ll.h) when included by third-party components.
#   The HAL header is not treated as a system header by GCC, so the
#   warning is not suppressed by the per-component -w flag above.
#   Valid for both C and C++; no generator expression required.
#
idf_build_set_property(COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-missing-field-initializers" APPEND)