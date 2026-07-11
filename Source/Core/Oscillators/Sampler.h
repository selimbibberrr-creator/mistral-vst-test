#pragma once

#include "Oscillator.h"
#include "../Envelopes/ADSR.h"
#include <vector>

class Sampler : public Oscillator
{
public:
    Sampler();
    ~Sampler() override;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope) override;

    // Sample loading
    bool loadSample(const juce::String& filePath);
    bool loadSample(const void* data, size_t dataSize);
    void unloadSample();

    // Setters
    void setPitch(float semitones); // Pitch shift in semitones
    void setStartPoint(float start); // 0.0 to 1.0
    void setEndPoint(float end); // 0.0 to 1.0
    void setReverse(bool shouldReverse);
    void setTimeStretch(float factor); // 0.5 to 2.0

    // Getters
    bool hasSampleLoaded() const { return sampleData != nullptr; }
    float getPitch() const { return pitchSemitones; }
    float getStartPoint() const { return startPoint; }
    float getEndPoint() const { return endPoint; }
    bool getReverse() const { return reverse; }
    float getTimeStretch() const { return timeStretch; }
    const juce::String& getSampleName() const { return sampleName; }

    // State management
    void saveState(juce::ValueTree& state) override;
    void loadState(const juce::ValueTree& state) override;

private:
    // Sample data
    std::vector<float> sampleData;
    juce::String sampleName;
    int sampleLength = 0;
    int sampleChannels = 1;
    int sampleRate = 44100;

    // Playback parameters
    float pitchSemitones = 0.0f;
    float startPoint = 0.0f;
    float endPoint = 1.0f;
    bool reverse = false;
    float timeStretch = 1.0f;

    // Playback state
    float playbackPosition = 0.0f;
    float playbackIncrement = 1.0f;
    bool isPlaying = false;

    // Resampling state
    float resamplePosition = 0.0f;
    float resampleIncrement = 1.0f;

    // Calculate playback increment based on pitch
    void updatePlaybackIncrement();

    // Get sample at position with interpolation
    float getSampleAtPosition(float position);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sampler)
};
