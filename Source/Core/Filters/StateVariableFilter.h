#pragma once

#include "Filter.h"

// State Variable Filter (SVF) implementation
// Supports LP/HP/BP/Notch with morphing between types
class StateVariableFilter : public Filter
{
public:
    StateVariableFilter();
    ~StateVariableFilter() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    float processSample(float sample, int channel) override;

    // Setters
    void setType(FilterType type) override;
    void setCutoff(float frequency) override;
    void setResonance(float resonance) override;
    void setSlope(Slope slope) override;

    // Morph between filter types
    void setMorph(float morphValue); // 0.0 = LP, 0.33 = BP, 0.66 = HP, 1.0 = Notch

    // Drive for nonlinear behavior
    void setDrive(float amount); // 0.0 to 1.0

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    // SVF state for each channel
    struct SVFState
    {
        float ic1eq = 0.0f; // Integrator 1
        float ic2eq = 0.0f; // Integrator 2
    };

    SVFState svfState[2]; // Stereo support

    // Morph parameters
    float morph = 0.0f; // 0.0 to 1.0
    float drive = 0.0f; // 0.0 to 1.0

    // Coefficients
    float g = 0.0f; // Feedback gain
    float k = 0.0f; // Frequency parameter
    float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f; // Output coefficients

    // Update coefficients
    void updateCoefficients();

    // Process a single sample through the SVF
    float processSampleInternal(float sample, SVFState& state);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateVariableFilter)
};
