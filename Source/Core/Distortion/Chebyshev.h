#pragma once

#include "DistortionAlgorithm.h"

// Chebyshev polynomial distortion
// Generates harmonics based on polynomial order
class Chebyshev : public DistortionAlgorithm
{
public:
    Chebyshev();
    ~Chebyshev() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix) override;
    float processSample(float sample, float drive) override;

    // Setters
    void setOrder(int order); // 2 to 10
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    int order = 5; // Chebyshev polynomial order (2-10)
    
    // Pre-computed coefficients for efficiency
    std::vector<float> coefficients;
    
    // Update coefficients based on order
    void updateCoefficients();
    
    // Calculate Chebyshev polynomial value
    float chebyshevPolynomial(float x, int n) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chebyshev)
};
