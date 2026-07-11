#pragma once

#include "Filter.h"

// Standard filter implementation (LP/HP/BP/Notch with 12/24/48dB slope)
class StandardFilter : public Filter
{
public:
    StandardFilter();
    ~StandardFilter() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    float processSample(float sample, int channel) override;

    // Setters
    void setType(FilterType type) override;
    void setCutoff(float frequency) override;
    void setResonance(float resonance) override;
    void setSlope(Slope slope) override;

    // Enable/disable key tracking
    void setKeyTracking(bool enabled);
    void setKeyTrackingAmount(float amount); // 0.0 to 1.0

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    // Filter state for each channel
    struct FilterState
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float z3 = 0.0f;
        float z4 = 0.0f;
    };

    FilterState filterState[2]; // Stereo support

    // Key tracking
    bool keyTrackingEnabled = false;
    float keyTrackingAmount = 1.0f;
    float lastNoteFrequency = 440.0f; // A4

    // Coefficients for different filter types and slopes
    void updateCoefficients();
    
    // Process a single sample through the filter
    float processSampleInternal(float sample, FilterState& state);

    // Coefficients for current filter configuration
    float a0 = 1.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, a4 = 0.0f;
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StandardFilter)
};
