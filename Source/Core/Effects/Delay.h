#pragma once

#include <JuceHeader.h>

// Delay effect with syncable time
class Delay
{
public:
    enum SyncMode
    {
        MS,    // Milliseconds
        BPM    // Beats per minute
    };

    Delay();
    ~Delay();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Setters
    void setTime(float time); // Time in ms or beats
    void setSyncMode(SyncMode mode);
    void setFeedback(float feedback); // 0.0 to 1.0
    void setFilter(float cutoff); // 0.0 to 1.0 (0 = no filter, 1 = full filter)
    void setBPM(float bpm); // For sync mode

    // Getters
    float getTime() const { return time; }
    SyncMode getSyncMode() const { return syncMode; }
    float getFeedback() const { return feedback; }
    float getFilter() const { return filterCutoff; }
    float getBPM() const { return bpm; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    float time = 250.0f; // ms or beats
    SyncMode syncMode = MS;
    float feedback = 0.5f; // 0.0 to 1.0
    float filterCutoff = 0.5f; // 0.0 to 1.0
    float bpm = 120.0f;

    // Delay buffers (stereo)
    juce::AudioBuffer<float> delayBuffer;
    int delayPosition = 0;
    int delaySamples = 0;

    // Filter state for each channel
    float filterState[2] = {0.0f, 0.0f};

    // Update delay time
    void updateDelayTime();

    // Process a single sample
    float processSample(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};
