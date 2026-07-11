#pragma once

#include "Oscillator.h"
#include "../Envelopes/ADSR.h"

class SubGenerator : public Oscillator
{
public:
    SubGenerator();
    ~SubGenerator() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) override;

    // Setters
    void setSaturation(float amount); // 0.0 to 1.0
    void setPhaseOffset(float offset); // 0.0 to 1.0

    // Getters
    float getSaturation() const { return saturation; }
    float getPhaseOffset() const { return phaseOffset; }

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    float saturation = 0.0f; // Amount of saturation to apply
    float phaseOffset = 0.0f; // Phase offset (0.0 to 1.0)

    // Generate a pure sine wave sample with optional saturation
    float generateSample();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubGenerator)
};
