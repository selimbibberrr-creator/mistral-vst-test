#include "Foldback.h"
#include <cmath>

Foldback::Foldback()
{
}

Foldback::~Foldback()
{
}

void Foldback::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update fold count based on drive
    int effectiveFoldCount = juce::jlimit(1, 10, static_cast<int>(juce::jmap(drive, 0.0f, 1.0f, 1.0f, 10.0f)));
    
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

float Foldback::processSample(float sample, float drive)
{
    // Update fold count based on drive
    int effectiveFoldCount = juce::jlimit(1, 10, static_cast<int>(juce::jmap(drive, 0.0f, 1.0f, 1.0f, 10.0f)));
    
    // Apply wavefolding
    float folded = foldWaveform(sample, effectiveFoldCount);
    
    // Mix between original and folded
    return sample * (1.0f - foldMix) + folded * foldMix;
}

void Foldback::setFoldCount(int count)
{
    foldCount = juce::jlimit(1, 10, count);
}

void Foldback::setMix(float foldMix)
{
    this->foldMix = juce::jlimit(0.0f, 1.0f, foldMix);
}

void Foldback::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: // Fold count
            foldCount = juce::jlimit(1, 10, static_cast<int>(value));
            break;
        case 1: // Mix
            foldMix = juce::jlimit(0.0f, 1.0f, value);
            break;
    }
}

float Foldback::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return static_cast<float>(foldCount);
        case 1: return foldMix;
        default: return 0.0f;
    }
}

float Foldback::foldWaveform(float x, int folds) const
{
    // Wavefolding implementation
    // Folds the waveform at multiple thresholds
    
    float result = x;
    float threshold = 1.0f / static_cast<float>(folds);
    
    for (int i = 1; i <= folds; ++i)
    {
        float currentThreshold = i * threshold;
        
        // Fold at positive threshold
        if (result > currentThreshold)
        {
            result = 2.0f * currentThreshold - result;
        }
        
        // Fold at negative threshold
        if (result < -currentThreshold)
        {
            result = -2.0f * currentThreshold - result;
        }
    }
    
    return result;
}

void Foldback::saveState(juce::ValueTree& state)
{
    state.setProperty("foldCount", foldCount, nullptr);
    state.setProperty("foldMix", foldMix, nullptr);
}

void Foldback::loadState(const juce::ValueTree& state)
{
    if (auto fc = state.getProperty("foldCount"))
        foldCount = static_cast<int>(*fc);
    
    if (auto fm = state.getProperty("foldMix"))
        foldMix = static_cast<float>(*fm);
}
