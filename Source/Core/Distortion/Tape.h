#pragma once

#include "DistortionAlgorithm.h"

// Tape saturation distortion
// Emulates analog tape saturation characteristics
class Tape : public DistortionAlgorithm
{
public:
    Tape();
    ~Tape() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) override;
    float processSample(float sample, float drive) override;

    // Setters
    void setBias(float bias); // 0.0 to 1.0
    void setHiss(float hiss); // 0.0 to 1.0
    void setWowFlutter(float amount); // 0.0 to 1.0
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    // Tape parameters
    float bias = 0.5f; // 0.0 to 1.0
    float hiss = 0.1f; // 0.0 to 1.0
    float wowFlutter = 0.0f; // 0.0 to 1.0
    
    // Tape saturation state
    float tapeState = 0.0f;
    
    // Wow/flutter state
    float lfoPhase = 0.0f;
    float lfoIncrement = 0.0f;
    float modulationDepth = 0.0f;
    
    // Noise state (for hiss)
    float noiseState1 = 0.0f;
    float noiseState2 = 0.0f;
    
    // Update LFO increment
    void updateLFO();
    
    // Generate tape saturation curve
    float tapeSaturation(float x);
    
    // Generate hiss noise
    float generateHiss();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tape)
};
