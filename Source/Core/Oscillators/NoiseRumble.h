#pragma once

#include "Oscillator.h"
#include "../Envelopes/ADSR.h"

class NoiseRumble : public Oscillator
{
public:
    enum FilterType
    {
        LOWPASS,
        HIGHPass,
        BANDPASS,
        NOTCH
    };

    NoiseRumble();
    ~NoiseRumble() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) override;

    // Setters
    void setFilterType(FilterType type);
    void setCutoffFrequency(float frequency);
    void setResonance(float resonance);
    void setDecayTime(float timeMs);

    // Getters
    FilterType getFilterType() const { return filterType; }
    float getCutoffFrequency() const { return cutoffFrequency; }
    float getResonance() const { return resonance; }
    float getDecayTime() const { return decayTime; }

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    FilterType filterType = BANDPASS;
    float cutoffFrequency = 500.0f;
    float resonance = 0.5f;
    float decayTime = 1000.0f; // 1 second

    // Filter state variables
    float filterState1 = 0.0f;
    float filterState2 = 0.0f;
    float filterState3 = 0.0f;
    float filterState4 = 0.0f;

    // Noise state
    float noiseSample = 0.0f;
    float pinkNoiseState1 = 0.0f;
    float pinkNoiseState2 = 0.0f;
    float pinkNoiseState3 = 0.0f;

    // Decay envelope
    float decayValue = 1.0f;
    float decayIncrement = 0.0f;

    // Generate noise sample
    float generateNoiseSample();
    
    // Apply filter to sample
    float applyFilter(float sample);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseRumble)
};
