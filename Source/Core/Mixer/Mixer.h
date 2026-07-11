#pragma once

#include <JuceHeader.h>

// 5-channel mixer for the sound sources
class Mixer
{
public:
    Mixer();
    ~Mixer();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& outputBuffer,
                      const juce::AudioBuffer<float>& osc1Buffer,
                      const juce::AudioBuffer<float>& osc2Buffer,
                      const juce::AudioBuffer<float>& noiseBuffer,
                      const juce::AudioBuffer<float>& samplerBuffer,
                      const juce::AudioBuffer<float>& subBuffer);

    // Channel settings
    void setLevel(int channel, float level); // 0.0 to 1.0
    void setPan(int channel, float pan); // 0.0 (left) to 1.0 (right)
    void setMute(int channel, bool mute);
    void setSolo(int channel, bool solo);

    // Getters
    float getLevel(int channel) const;
    float getPan(int channel) const;
    bool getMute(int channel) const;
    bool getSolo(int channel) const;

    // Global settings
    void setMasterLevel(float level); // 0.0 to 1.0
    void setMasterPan(float pan); // 0.0 to 1.0

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    // Channel settings (5 channels: OSC1, OSC2, NOISE, SAMPLER, SUB)
    struct ChannelSettings
    {
        float level = 1.0f;
        float pan = 0.5f; // 0.0 = left, 1.0 = right
        bool mute = false;
        bool solo = false;
    };

    ChannelSettings channels[5];
    
    // Master settings
    float masterLevel = 1.0f;
    float masterPan = 0.5f;

    // Solo state
    bool anySolo = false;

    // Temporary buffer for mixing
    juce::AudioBuffer<float> mixBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
