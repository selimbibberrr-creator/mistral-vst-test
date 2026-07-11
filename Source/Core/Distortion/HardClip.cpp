#include "HardClip.h"
#include <cmath>

HardClip::HardClip()
{
}

HardClip::~HardClip()
{
}

void HardClip::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Calculate effective threshold based on drive
    float effectiveThreshold = juce::jmap(drive, 0.0f, 1.0f, 0.9f, 0.1f);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float original = channelData[sample];
            float distorted = processSample(original, drive);
            channelData[sample] = original * (1.0f - mix) + distorted * mix;
        }
    }
}

float HardClip::processSample(float sample, float drive)
{
    // Calculate effective threshold based on drive
    float effectiveThreshold = juce::jmap(drive, 0.0f, 1.0f, 0.9f, 0.1f);
    
    // Apply asymmetry
    float positiveThreshold = effectiveThreshold * (1.0f - asymmetry * 0.5f);
    float negativeThreshold = effectiveThreshold * (1.0f + asymmetry * 0.5f);
    
    // Hard clipping with asymmetry
    if (sample > positiveThreshold)
        return positiveThreshold;
    else if (sample < -negativeThreshold)
        return -negativeThreshold;
    else
        return sample;
}

void HardClip::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: // Threshold
            threshold = juce::jlimit(0.01f, 0.99f, value);
            break;
        case 1: // Asymmetry
            asymmetry = juce::jlimit(-1.0f, 1.0f, value);
            break;
    }
}

float HardClip::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return threshold;
        case 1: return asymmetry;
        default: return 0.0f;
    }
}

void HardClip::saveState(juce::ValueTree& state)
{
    state.setProperty("threshold", threshold, nullptr);
    state.setProperty("asymmetry", asymmetry, nullptr);
}

void HardClip::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("threshold"))
        threshold = static_cast<float>(*t);
    
    if (auto a = state.getProperty("asymmetry"))
        asymmetry = static_cast<float>(*a);
}
