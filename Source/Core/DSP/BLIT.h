#pragma once

#include <JuceHeader.h>

// Band-Limited Impulse Train (BLIT) for anti-aliased oscillators
class BLIT
{
public:
    BLIT();
    ~BLIT();

    // Initialize
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Set frequency
    void setFrequency(float frequency);

    // Generate a sample
    float generateSample();

    // Generate a block of samples
    void generateBlock(float* output, int numSamples);

    // Set waveform type
    void setWaveform(int type); // 0 = saw, 1 = square, 2 = pulse

    // Set pulse width (for pulse wave)
    void setPulseWidth(float width);

private:
    double sampleRate = 44100.0;
    float frequency = 440.0f;
    int waveformType = 0; // 0 = saw, 1 = square, 2 = pulse
    float pulseWidth = 0.5f;

    // BLIT state
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    
    // Number of harmonics to generate
    int numHarmonics = 20;
    
    // Harmonic state
    struct Harmonic
    {
        float phase = 0.0f;
        float phaseIncrement = 0.0f;
        float amplitude = 0.0f;
    };
    
    std::vector<Harmonic> harmonics;

    // Update harmonic parameters
    void updateHarmonics();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BLIT)
};
