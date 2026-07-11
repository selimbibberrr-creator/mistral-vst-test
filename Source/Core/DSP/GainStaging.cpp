#include "GainStaging.h"
#include <cmath>

void GainStaging::normalizeBuffer(juce::AudioBuffer<float>& buffer, float headroom)
{
    float peak = calculatePeak(buffer);
    
    if (peak > 0.0f)
    {
        float gain = headroom / peak;
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= gain;
            }
        }
    }
}

bool GainStaging::applyGainWithClipDetection(juce::AudioBuffer<float>& buffer, float gain)
{
    bool clipped = false;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gain;
            
            if (std::abs(channelData[sample]) > 0.99f)
            {
                clipped = true;
            }
        }
    }
    
    return clipped;
}

float GainStaging::dbToGain(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

float GainStaging::gainToDb(float gain)
{
    if (gain <= 0.0f)
        return -100.0f; // Very low gain
    
    return 20.0f * std::log10(gain);
}

float GainStaging::calculateRMS(const juce::AudioBuffer<float>& buffer)
{
    float sumOfSquares = 0.0f;
    int totalSamples = 0;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float sampleValue = channelData[sample];
            sumOfSquares += sampleValue * sampleValue;
            totalSamples++;
        }
    }
    
    if (totalSamples > 0)
    {
        float meanSquare = sumOfSquares / totalSamples;
        return std::sqrt(meanSquare);
    }
    
    return 0.0f;
}

float GainStaging::calculatePeak(const juce::AudioBuffer<float>& buffer)
{
    float peak = 0.0f;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float absSample = std::abs(channelData[sample]);
            if (absSample > peak)
                peak = absSample;
        }
    }
    
    return peak;
}

void GainStaging::applyMakeupGain(juce::AudioBuffer<float>& buffer, float targetRMS)
{
    float currentRMS = calculateRMS(buffer);
    
    if (currentRMS > 0.0f && targetRMS > 0.0f)
    {
        float gain = targetRMS / currentRMS;
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= gain;
            }
        }
    }
}

void GainStaging::softClipBuffer(juce::AudioBuffer<float>& buffer, float threshold)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float& sampleValue = channelData[sample];
            
            if (sampleValue > threshold)
            {
                sampleValue = threshold + (sampleValue - threshold) / (1.0f + (sampleValue - threshold) * 10.0f);
            }
            else if (sampleValue < -threshold)
            {
                sampleValue = -threshold - (sampleValue + threshold) / (1.0f + (sampleValue + threshold) * 10.0f);
            }
        }
    }
}

void GainStaging::hardClipBuffer(juce::AudioBuffer<float>& buffer, float threshold)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float& sampleValue = channelData[sample];
            
            if (sampleValue > threshold)
                sampleValue = threshold;
            else if (sampleValue < -threshold)
                sampleValue = -threshold;
        }
    }
}

GainStaging::GainStaging()
{
}

GainStaging::~GainStaging()
{
}
