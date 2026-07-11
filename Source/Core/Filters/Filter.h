#pragma once

#include <JuceHeader.h>

// Base filter class
class Filter
{
public:
    enum FilterType
    {
        LOWPASS,
        HIGHPass,
        BANDPASS,
        NOTCH
    };

    enum Slope
    {
        DB12,
        DB24,
        DB48
    };

    Filter();
    virtual ~Filter();

    // Lifecycle
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock);
    virtual void releaseResources();

    // Processing
    virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
    virtual float processSample(float sample, int channel) = 0;

    // Setters
    virtual void setType(FilterType type);
    virtual void setCutoff(float frequency);
    virtual void setResonance(float resonance);
    virtual void setSlope(Slope slope);

    // Getters
    FilterType getType() const { return type; }
    float getCutoff() const { return cutoffFrequency; }
    float getResonance() const { return resonance; }
    Slope getSlope() const { return slope; }

    // State management
    virtual void saveState(juce::ValueTree& state);
    virtual void loadState(const juce::ValueTree& state);

protected:
    double sampleRate = 44100.0;
    int blockSize = 512;

    FilterType type = LOWPASS;
    float cutoffFrequency = 1000.0f;
    float resonance = 0.5f;
    Slope slope = DB12;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filter)
};
