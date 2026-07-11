#pragma once

#include "Oscillator.h"
#include "../Envelopes/ADSR.h"

class Oscillator1 : public Oscillator
{
public:
    Oscillator1();
    ~Oscillator1() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) override;

    // Setters
    void setPulseWidth(float width); // For square wave PWM (0.0 to 1.0)
    void setDetune(float cents); // Detune in cents (-50 to +50)
    void setPhase(float phaseOffset); // Phase offset (0.0 to 1.0)

    // Getters
    float getPulseWidth() const { return pulseWidth; }
    float getDetune() const { return detuneCents; }
    float getPhase() const { return phaseOffset; }

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    float pulseWidth = 0.5f; // 50% duty cycle
    float detuneCents = 0.0f;
    float phaseOffset = 0.0f;

    // For oversampling (2x)
    juce::AudioBuffer<float> oversampledBuffer;

    // Generate a single sample
    float generateSample();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator1)
};
