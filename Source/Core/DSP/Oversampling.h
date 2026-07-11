#pragma once

#include <JuceHeader.h>

// Oversampling utility class
class Oversampler
{
public:
    Oversampler();
    ~Oversampler();

    // Initialize with oversampling factor
    void prepareToPlay(double sampleRate, int samplesPerBlock, int factor = 2);
    void releaseResources();

    // Process buffer with oversampling
    void processBlock(juce::AudioBuffer<float>& buffer, 
                     std::function<void(juce::AudioBuffer<float>&)> processFunction);

    // Set oversampling factor (1x, 2x, 4x)
    void setFactor(int factor);
    int getFactor() const { return oversamplingFactor; }

private:
    double sampleRate = 44100.0;
    int blockSize = 512;
    int oversamplingFactor = 2;

    // Buffers for oversampling
    juce::AudioBuffer<float> upsampledBuffer;
    juce::AudioBuffer<float> downsampledBuffer;

    // Resampling state
    void upsample(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output);
    void downsample(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oversampler)
};
