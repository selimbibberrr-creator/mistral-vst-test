#pragma once

#include "DistortionAlgorithm.h"

// Foldback (wavefolding) distortion
// Creates extreme harmonics by folding the waveform
class Foldback : public DistortionAlgorithm
{
public:
    Foldback();
    ~Foldback() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) override;
    float processSample(float sample, float drive) override;

    // Setters
    void setFoldCount(int count); // 1 to 10 folds
    void setMix(float foldMix); // 0.0 to 1.0 (mix between original and folded)
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    int foldCount = 3; // Number of folds (1-10)
    float foldMix = 0.5f; // Mix between original and folded signal
    
    // Wavefolding implementation
    float foldWaveform(float x, int folds) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Foldback)
};
