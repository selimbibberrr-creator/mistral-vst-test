#pragma once

#include "DistortionAlgorithm.h"

// Bitcrush distortion (digital degradation)
class Bitcrush : public DistortionAlgorithm
{
public:
    Bitcrush();
    ~Bitcrush() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) override;
    float processSample(float sample, float drive) override;

    // Setters
    void setBitDepth(int depth); // 1 to 16 bits
    void setSampleRate(float rate); // 1000 to 44100 Hz
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    int bitDepth = 8; // 1-16 bits
    float crushSampleRate = 12000.0f; // 1k-44.1k Hz
    
    // Sample rate reduction state
    float sampleCounter = 0.0f;
    float sampleIncrement = 1.0f;
    float lastSample = 0.0f;

    // Update sample increment based on crush sample rate
    void updateSampleIncrement();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Bitcrush)
};
