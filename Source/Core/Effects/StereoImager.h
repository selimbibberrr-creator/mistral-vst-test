#pragma once

#include <JuceHeader.h>

// Stereo imaging effect (Mid/Side processing)
class StereoImager
{
public:
    StereoImager();
    ~StereoImager();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Setters
    void setWidth(float width); // 0.0 to 1.0 (0 = mono, 1 = max stereo)
    void setFocus(float focus); // 0.0 to 1.0 (0 = wide, 1 = narrow)
    void setPhaseCorrection(bool enabled);

    // Getters
    float getWidth() const { return width; }
    float getFocus() const { return focus; }
    bool getPhaseCorrection() const { return phaseCorrection; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    float width = 0.5f; // 0.0 to 1.0
    float focus = 0.5f; // 0.0 to 1.0
    bool phaseCorrection = true;

    // Allpass filters for phase correction
    std::vector<float> allpassLines[2]; // Left and right
    int allpassPositions[2] = {0, 0};

    // Update allpass filters
    void updateAllpassFilters();

    // Process a single sample
    float processSample(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoImager)
};
