#pragma once

#include <JuceHeader.h>

// DC blocking filter to remove DC offset from audio signals
class DCBlocker
{
public:
    DCBlocker();
    ~DCBlocker();

    // Initialize
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Process a single sample
    float processSample(float sample, int channel);

    // Process a buffer
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Reset the filter state
    void reset();

private:
    double sampleRate = 44100.0;
    
    // Filter state for each channel
    float state[2] = {0.0f, 0.0f};
    
    // Filter coefficient
    float coefficient = 0.999f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DCBlocker)
};
