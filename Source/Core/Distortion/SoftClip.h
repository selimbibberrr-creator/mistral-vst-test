#pragma once

#include "DistortionAlgorithm.h"

// Soft clipping distortion (warm saturation)
class SoftClip : public DistortionAlgorithm
{
public:
    SoftClip();
    ~SoftClip() override;

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
    // Soft clip parameters
    float threshold = 0.8f; // Clipping threshold (0.0 to 1.0)
    float curve = 2.0f; // Curve amount (1.0 = linear, 2.0 = quadratic, etc.)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoftClip)
};
