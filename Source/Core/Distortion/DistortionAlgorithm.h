#pragma once

#include <JuceHeader.h>

// Base class for distortion algorithms
class DistortionAlgorithm
{
public:
    DistortionAlgorithm();
    virtual ~DistortionAlgorithm();

    // Lifecycle
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock);
    virtual void releaseResources();

    // Processing
    virtual void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) = 0;
    virtual float processSample(float sample, float drive) = 0;

    // Setters
    virtual void setParameter(int parameterIndex, float value) {}
    virtual float getParameter(int parameterIndex) const { return 0.0f; }

    // State management
    virtual void saveState(juce::ValueTree& state);
    virtual void loadState(const juce::ValueTree& state);

protected:
    double sampleRate = 44100.0;
    int blockSize = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionAlgorithm)
};
