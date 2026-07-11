cmake_minimum_required(VERSION 3.15)

# Project name and version
project(HardeKickSynth VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard (C++17 for JUCE compatibility)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    Source/Core/Filters/FilterBank.cpp
    Source/Core/Filters/StateVariableFilter.cpp
    Source/Core/Distortion/DistortionUnit.cpp
    Source/Core/Distortion/MultibandDistortion.cpp
    Source/Core/Effects/Compressor.cpp
    Source/Core/Effects/ParametricEQ.cpp
    Source/Core/Effects/Reverb.cpp
    Source/Core/Effects/Delay.cpp
    Source/Core/Effects/StereoImager.cpp
    Source/Core/Routing/RoutingMatrix.cpp
    Source/UI/Components/Oscilloscope.cpp
    Source/UI/Components/SpectrumAnalyzer.cpp
    Source/UI/Components/ADSREditor.cpp
    Source/UI/MainWindow.cpp
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
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
