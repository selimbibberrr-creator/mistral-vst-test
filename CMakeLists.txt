cmake_minimum_required(VERSION 3.15)

# Project name and version
project(HardeKickSynth VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard (C++17 for JUCE compatibility)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable optimizations
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug/Release)")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Enable SIMD instructions
if(MSVC)
    add_compile_options(/arch:SSE2)
    add_definitions(-D__SSE__ -D__SSE2__)
else()
    add_compile_options(-msse -msse2 -mfpmath=sse)
    add_definitions(-D__SSE__ -D__SSE2__)
endif()

# Check for AVX support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-mavx" HAS_AVX_FLAG)
if(HAS_AVX_FLAG)
    add_compile_options(-mavx)
    add_definitions(-D__AVX__)
endif()

# Find JUCE (assuming it's installed in /opt/JUCE or specified via JUCE_PATH)
set(JUCE_PATH "$ENV{JUCE_PATH}" CACHE PATH "Path to JUCE directory")
if(NOT JUCE_PATH)
    set(JUCE_PATH "/opt/JUCE" CACHE PATH "Default JUCE path")
endif()

# Include JUCE modules
list(APPEND CMAKE_MODULE_PATH "${JUCE_PATH}/modules")
include_directories(
    ${JUCE_PATH}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Add JUCE as a dependency (if built as a library)
# For now, we'll include JUCE headers directly
add_definitions(-DJUCE_AMALGAMATED_INCLUDE=0)

# Source files
set(SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Core/AudioEngine.cpp
    Source/Core/Oscillators/Oscillator.cpp
    Source/Core/Oscillators/Oscillator1.cpp
    Source/Core/Oscillators/Oscillator2.cpp
    Source/Core/Oscillators/NoiseRumble.cpp
    Source/Core/Oscillators/Sampler.cpp
    Source/Core/Oscillators/SubGenerator.cpp
    Source/Core/Envelopes/ADSR.cpp
    Source/Core/Envelopes/VisualShaper.cpp
    Source/Core/Mixer/Mixer.cpp
    Source/Core/Filters/Filter.cpp
    Source/Core/Filters/FilterBank.cpp
    Source/Core/Filters/StandardFilter.cpp
    Source/Core/Filters/StateVariableFilter.cpp
    Source/Core/Distortion/DistortionAlgorithm.cpp
    Source/Core/Distortion/DistortionUnit.cpp
    Source/Core/Distortion/SoftClip.cpp
    Source/Core/Distortion/HardClip.cpp
    Source/Core/Distortion/Chebyshev.cpp
    Source/Core/Distortion/Bitcrush.cpp
    Source/Core/Distortion/Tape.cpp
    Source/Core/Distortion/Foldback.cpp
    Source/Core/Effects/Compressor.cpp
    Source/Core/Effects/ParametricEQ.cpp
    Source/Core/Effects/Reverb.cpp
    Source/Core/Effects/Delay.cpp
    Source/Core/Effects/StereoImager.cpp
    Source/Core/Routing/RoutingMatrix.cpp
    Source/Core/Presets/PresetManager.cpp
    Source/Core/DSP/Oversampling.cpp
    Source/Core/DSP/SimdUtils.cpp
    Source/Core/DSP/DCBlocker.cpp
    Source/Core/DSP/BLIT.cpp
    Source/Core/DSP/GainStaging.cpp
    Source/Core/DSP/LatencyCompensator.cpp
    Source/UI/Components/Oscilloscope.cpp
    Source/UI/Components/SpectrumAnalyzer.cpp
    Source/UI/Components/ADSREditor.cpp
    Source/UI/MainWindow.cpp
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Oscillators
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Envelopes
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Filters
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Distortion
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Effects
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Mixer
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Routing
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Presets
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI/Components
)

# Create the VST3 plugin
juce_add_plugin(HardeKickSynth
    PRODUCT_NAME "Harde Kick Synth"
    PLUGIN_MANUFACTURER_CODE "Selim"
    PLUGIN_CODE "HrdK"
    FORMATS VST3
    SOURCES ${SOURCES}
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# If JUCE is not found, create a fallback target for testing
if(NOT TARGET HardeKickSynth)
    message(WARNING "JUCE not found. Creating a fallback executable for testing.")
    add_executable(HardeKickSynth ${SOURCES})
    target_include_directories(HardeKickSynth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
    target_compile_definitions(HardeKickSynth PRIVATE JUCE_DONT_DECLARE_PROJECT_TYPES)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(HardeKickSynth PRIVATE /W4)
else()
    target_compile_options(HardeKickSynth PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable optimizations for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(HardeKickSynth PRIVATE -O3 -DNDEBUG)
    if(MSVC)
        target_compile_options(HardeKickSynth PRIVATE /O2 /GL)
    endif()
endif()

# Enable SIMD for the target
if(MSVC)
    target_compile_options(HardeKickSynth PRIVATE /arch:SSE2)
else()
    target_compile_options(HardeKickSynth PRIVATE -msse -msse2)
    if(HAS_AVX_FLAG)
        target_compile_options(HardeKickSynth PRIVATE -mavx)
    endif()
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)
