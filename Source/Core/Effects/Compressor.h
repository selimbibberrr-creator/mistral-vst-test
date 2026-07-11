#pragma once

#include <JuceHeader.h>

// Compressor effect with sidechain support
class Compressor
{
public:
    enum CompressorType
    {
        VCA,
        FET,
        OPTO
    };

    Compressor();
    ~Compressor();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Setters
    void setType(CompressorType type);
    void setThreshold(float threshold); // dB
    void setRatio(float ratio); // 1.0 to infinity
    void setAttack(float attack); // ms
    void setRelease(float release); // ms
    void setMakeupGain(float gain); // dB
    void setSidechainEnabled(bool enabled);
    void setSidechainInput(const juce::AudioBuffer<float>& sidechainBuffer);

    // Getters
    CompressorType getType() const { return type; }
    float getThreshold() const { return threshold; }
    float getRatio() const { return ratio; }
    float getAttack() const { return attack; }
    float getRelease() const { return release; }
    float getMakeupGain() const { return makeupGain; }
    bool isSidechainEnabled() const { return sidechainEnabled; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    CompressorType type = VCA;
    float threshold = -12.0f; // dB
    float ratio = 4.0f; // 4:1
    float attack = 10.0f; // 10ms
    float release = 100.0f; // 100ms
    float makeupGain = 0.0f; // 0dB
    bool sidechainEnabled = false;

    // Internal state
    float envelope = 0.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;

    // Sidechain
    const juce::AudioBuffer<float>* sidechainBuffer = nullptr;

    // Update coefficients
    void updateCoefficients();

    // Process a single sample
    float processSample(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};
