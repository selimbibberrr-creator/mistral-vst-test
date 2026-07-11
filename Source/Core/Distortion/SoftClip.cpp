#include "SoftClip.h"
#include <cmath>

SoftClip::SoftClip()
{
}

SoftClip::~SoftClip()
{
}

void SoftClip::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Calculate effective threshold based on drive
    float effectiveThreshold = juce::jmap(drive, 0.0f, 1.0f, 0.9f, 0.3f);
    
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

float SoftClip::processSample(float sample, float drive)
{
    // Calculate effective threshold based on drive
    float effectiveThreshold = juce::jmap(drive, 0.0f, 1.0f, 0.9f, 0.3f);
    
    // Soft clipping using a smooth curve
    // This implements a polynomial soft clip: y = x / (1 + |x|^n) * (1 + |x|)
    // where n controls the curve sharpness
    
    float absSample = std::abs(sample);
    
    if (absSample < effectiveThreshold)
    {
        // Below threshold, apply gentle saturation
        float x = sample / effectiveThreshold;
        float x2 = x * x;
        float x4 = x2 * x2;
        
        // Polynomial approximation of soft clip
        float saturated = x * (1.0f + x2 * (-0.5f + x2 * 0.166667f));
        return saturated * effectiveThreshold;
    }
    else
    {
        // Above threshold, apply stronger saturation
        float sign = (sample > 0) ? 1.0f : -1.0f;
        float over = absSample - effectiveThreshold;
        float overNormalized = over / (1.0f - effectiveThreshold);
        
        // Apply curve to the overshoot
        float clippedOver = overNormalized / (1.0f + overNormalized * curve);
        
        return sign * (effectiveThreshold + clippedOver * (1.0f - effectiveThreshold));
    }
}

void SoftClip::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: // Threshold
            threshold = juce::jlimit(0.1f, 0.99f, value);
            break;
        case 1: // Curve
            curve = juce::jlimit(1.0f, 10.0f, value);
            break;
    }
}

float SoftClip::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return threshold;
        case 1: return curve;
        default: return 0.0f;
    }
}

void SoftClip::saveState(juce::ValueTree& state)
{
    state.setProperty("threshold", threshold, nullptr);
    state.setProperty("curve", curve, nullptr);
}

void SoftClip::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("threshold"))
        threshold = static_cast<float>(*t);
    
    if (auto c = state.getProperty("curve"))
        curve = static_cast<float>(*c);
}
