#pragma once

#include "Oscillator.h"
#include "../Envelopes/ADSR.h"

class Oscillator2 : public Oscillator
{
public:
    enum WavetableType
    {
        SINE_TO_SQUARE,
        SINE_TO_SAW,
        SQUARE_TO_SAW,
        CUSTOM
    };

    Oscillator2();
    ~Oscillator2() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) override;

    // Setters
    void setWavetableType(WavetableType type);
    void setWavetablePosition(float position); // 0.0 to 1.0
    void setFMIndex(float index); // FM modulation index
    void setModulatorFrequency(float freq); // For FM synthesis
    void setNoiseColor(int color); // 0=White, 1=Pink, 2=Brown

    // Getters
    WavetableType getWavetableType() const { return wavetableType; }
    float getWavetablePosition() const { return wavetablePosition; }
    float getFMIndex() const { return fmIndex; }
    float getModulatorFrequency() const { return modulatorFrequency; }
    int getNoiseColor() const { return noiseColor; }

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    WavetableType wavetableType = SINE_TO_SQUARE;
    float wavetablePosition = 0.5f;
    float fmIndex = 0.0f;
    float modulatorFrequency = 440.0f;
    int noiseColor = 0; // 0=White, 1=Pink, 2=Brown

    // For FM synthesis
    float modulatorPhase = 0.0f;
    float modulatorPhaseIncrement = 0.0f;

    // For noise generation
    float noiseSample = 0.0f;
    float pinkNoiseState1 = 0.0f;
    float pinkNoiseState2 = 0.0f;
    float pinkNoiseState3 = 0.0f;

    // Generate a single sample
    float generateSample();
    float generateWavetableSample(float position);
    float generateFMSample();
    float generateNoiseSample();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator2)
};
