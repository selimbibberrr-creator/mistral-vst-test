#pragma once

#include "DistortionAlgorithm.h"

// Hard clipping distortion (aggressive)
class HardClip : public DistortionAlgorithm
{
public:
    HardClip();
    ~HardClip() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) override;
    float processSample(float sample, float drive) override;

    // Setters
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    // Hard clip parameters
    float threshold = 0.8f; // Clipping threshold (0.0 to 1.0)
    float asymmetry = 0.0f; // Asymmetry (-1.0 to 1.0)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardClip)
};
