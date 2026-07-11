#include "DCBlocker.h"

DCBlocker::DCBlocker()
{
}

DCBlocker::~DCBlocker()
{
}

void DCBlocker::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    // Calculate coefficient based on sample rate
    // Higher sample rate = closer to 1.0
    coefficient = 1.0f - 1.0f / (sampleRate * 0.001f);
    reset();
}

void DCBlocker::releaseResources()
{
    reset();
}

float DCBlocker::processSample(float sample, int channel)
{
    // High-pass filter at ~1Hz to remove DC
    // y[n] = x[n] - x[n-1] + coefficient * y[n-1]
    float output = sample - state[channel] + coefficient * state[channel];
    state[channel] = sample;
    return output;
}

void DCBlocker::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSample(channelData[sample], channel);
        }
    }
}

void DCBlocker::reset()
{
    state[0] = 0.0f;
    state[1] = 0.0f;
}
