#pragma once

#include <JuceHeader.h>
#include "../Source/Core/AudioEngine.h"

// Test harness for Harde Kick Synth
class TestHarness
{
public:
    TestHarness();
    ~TestHarness();

    // Run all tests
    void runAllTests();

    // Individual test functions
    void testOscillators();
    void testADSR();
    void testFilters();
    void testDistortion();
    void testEffects();
    void testMixer();
    void testPresets();
    void testPerformance();

    // Test results
    struct TestResult
    {
        juce::String testName;
        bool passed = false;
        juce::String message;
        float executionTime = 0.0f; // ms
    };

    const juce::Array<TestResult>& getResults() const { return results; }

private:
    std::unique_ptr<AudioEngine> audioEngine;
    juce::Array<TestResult> results;

    // Helper functions
    void addResult(const juce::String& testName, bool passed, const juce::String& message = "");
    
    // Create a test buffer
    juce::AudioBuffer<float> createTestBuffer(int numChannels, int numSamples);
    
    // Measure execution time
    float measureExecutionTime(std::function<void()> function);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestHarness)
};
