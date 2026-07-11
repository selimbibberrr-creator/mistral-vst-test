#pragma once

#include <JuceHeader.h>

// Parametric EQ with 6 bands
class ParametricEQ
{
public:
    enum BandType
    {
        BELL,
        NOTCH,
        LOW_SHELF,
        HIGH_SHELF,
        LOW_PASS,
        HIGH_PASS
    };

    struct EQBand
    {
        BandType type = BELL;
        float frequency = 1000.0f; // Hz
        float q = 1.0f; // Quality factor / bandwidth
        float gain = 0.0f; // dB
        bool enabled = true;
    };

    ParametricEQ();
    ~ParametricEQ();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Band management
    void setBand(int bandIndex, BandType type, float frequency, float q, float gain);
    EQBand& getBand(int bandIndex);
    const EQBand& getBand(int bandIndex) const;
    void enableBand(int bandIndex, bool enabled);

    // Global settings
    void setOversampling(int factor); // 1x, 2x
    void setLinearPhase(bool enabled);

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    EQBand bands[6];
    int oversampling = 1;
    bool linearPhase = false;

    // Filter state for each band and channel
    struct FilterState
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };

    FilterState filterStates[6][2]; // 6 bands, 2 channels

    // Temporary buffers for oversampling
    juce::AudioBuffer<float> oversampledBuffer;
    juce::AudioBuffer<float> downsampledBuffer;

    // Update coefficients for a band
    void updateBandCoefficients(int bandIndex);

    // Process a single sample through all bands
    float processSample(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEQ)
};
