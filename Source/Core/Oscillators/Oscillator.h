#pragma once

#include <JuceHeader.h>

// Base oscillator class
class Oscillator
{
public:
    enum WaveformType
    {
        SINE,
        SQUARE,
        SAW,
        TRIANGLE,
        NOISE,
        WAVETABLE,
        FM
    };

    Oscillator();
    virtual ~Oscillator();

    // Lifecycle
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock);
    virtual void releaseResources();

    // Processing
    virtual void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) = 0;

    // Note handling
    virtual void noteOn(int noteNumber, float velocity);
    virtual void noteOff();

    // Setters
    virtual void setWaveform(WaveformType type);
    virtual void setFrequency(float frequency);
    virtual void setPitchBend(float bend);
    virtual void setLevel(float level);
    virtual void setPan(float pan);

    // Getters
    WaveformType getWaveform() const { return waveformType; }
    float getFrequency() const { return frequency; }
    float getLevel() const { return level; }
    float getPan() const { return pan; }

    // State management
    virtual void saveState(juce::ValueTree& state);
    virtual void loadState(const juce::ValueTree& state);

protected:
    double sampleRate = 44100.0;
    int blockSize = 512;
    
    WaveformType waveformType = SINE;
    float frequency = 440.0f;
    float pitchBend = 0.0f;
    float level = 1.0f;
    float pan = 0.5f; // 0.0 = left, 1.0 = right
    
    bool isNoteOn = false;
    int currentNote = 60; // Middle C
    float currentVelocity = 1.0f;

    // Phase for oscillators
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator)
};
