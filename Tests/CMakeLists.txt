cmake_minimum_required(VERSION 3.15)

# Test project
project(HardeKickSynthTests VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add test executable
add_executable(HardeKickSynthTests
    TestHarness.cpp
    TestHarness.h
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Oscillators
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Envelopes
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Filters
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Distortion
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Effects
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Mixer
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Routing
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/Presets
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source/Core/DSP
)

# Link with JUCE (if available)
# For now, just compile as a standalone test

# Enable warnings
if(MSVC)
    target_compile_options(HardeKickSynthTests PRIVATE /W4)
else()
    target_compile_options(HardeKickSynthTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
