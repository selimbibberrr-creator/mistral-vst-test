#pragma once

#include <JuceHeader.h>

// Gain staging utilities for proper audio level management
class GainStaging
{
public:
    // Normalize a buffer to prevent clipping
    static void normalizeBuffer(juce::AudioBuffer<float>& buffer, float headroom = 0.9f);

    // Apply gain with clipping detection
    static bool applyGainWithClipDetection(juce::AudioBuffer<float>& buffer, float gain);

    // Convert dB to linear gain
    static float dbToGain(float db);

    // Convert linear gain to dB
    static float gainToDb(float gain);

    // Calculate RMS level of a buffer
    static float calculateRMS(const juce::AudioBuffer<float>& buffer);

    // Calculate peak level of a buffer
    static float calculatePeak(const juce::AudioBuffer<float>& buffer);

    // Apply makeup gain to compensate for loss
    static void applyMakeupGain(juce::AudioBuffer<float>& buffer, float targetRMS);

    // Soft clip to prevent digital clipping
    static void softClipBuffer(juce::AudioBuffer<float>& buffer, float threshold = 0.9f);

    // Hard clip to prevent digital clipping
    static void hardClipBuffer(juce::AudioBuffer<float>& buffer, float threshold = 0.99f);

private:
    GainStaging();
    ~GainStaging();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainStaging)
};
