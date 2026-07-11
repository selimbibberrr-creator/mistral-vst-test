#include "Tape.h"
#include <cmath>
#include <random>

Tape::Tape()
{
    updateLFO();
}

Tape::~Tape()
{
}

void Tape::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

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

float Tape::processSample(float sample, float drive)
{
    // Update LFO phase
    lfoPhase += lfoIncrement;
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    
    // Calculate modulation from wow/flutter
    float modulation = modulationDepth * std::sin(2.0f * juce::MathConstants<float>::pi * lfoPhase);
    
    // Apply tape saturation
    float saturated = tapeSaturation(sample * (1.0f + modulation));
    
    // Add hiss
    saturated += generateHiss() * hiss * 0.1f;
    
    // Apply bias (DC offset)
    saturated += bias * 0.1f * drive;
    
    return saturated;
}

void Tape::setBias(float newBias)
{
    bias = juce::jlimit(0.0f, 1.0f, newBias);
}

void Tape::setHiss(float newHiss)
{
    hiss = juce::jlimit(0.0f, 1.0f, newHiss);
}

void Tape::setWowFlutter(float amount)
{
    wowFlutter = juce::jlimit(0.0f, 1.0f, amount);
    modulationDepth = wowFlutter * 0.1f; // 10% modulation max
    updateLFO();
}

void Tape::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: // Bias
            bias = juce::jlimit(0.0f, 1.0f, value);
            break;
        case 1: // Hiss
            hiss = juce::jlimit(0.0f, 1.0f, value);
            break;
        case 2: // Wow/Flutter
            wowFlutter = juce::jlimit(0.0f, 1.0f, value);
            modulationDepth = wowFlutter * 0.1f;
            updateLFO();
            break;
    }
}

float Tape::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return bias;
        case 1: return hiss;
        case 2: return wowFlutter;
        default: return 0.0f;
    }
}

void Tape::updateLFO()
{
    // LFO at ~0.5Hz for wow/flutter
    lfoIncrement = 0.5f / static_cast<float>(sampleRate);
}

float Tape::tapeSaturation(float x)
{
    // Tape saturation curve
    // Soft clipping with asymmetric behavior
    
    float absX = std::abs(x);
    
    if (absX < 0.5f)
    {
        // Linear region
        return x;
    }
    else if (absX < 0.8f)
    {
        // Gentle saturation
        float sign = (x > 0) ? 1.0f : -1.0f;
        float over = absX - 0.5f;
        float normalizedOver = over / 0.3f;
        return sign * (0.5f + 0.3f * (1.0f - std::exp(-normalizedOver * 3.0f)));
    }
    else
    {
        // Strong saturation
        float sign = (x > 0) ? 1.0f : -1.0f;
        float over = absX - 0.8f;
        float normalizedOver = over / 0.2f;
        return sign * (0.8f + 0.2f * (1.0f - std::exp(-normalizedOver * 5.0f)));
    }
}

float Tape::generateHiss()
{
    // Simple pink noise for hiss
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    float white = dist(gen);
    noiseState1 = 0.997f * noiseState1 + 0.029f * white;
    noiseState2 = 0.985f * noiseState2 + 0.043f * white;
    noiseState3 = 0.950f * noiseState3 + 0.071f * white;
    return noiseState1 + noiseState2 + noiseState3;
}

void Tape::saveState(juce::ValueTree& state)
{
    state.setProperty("bias", bias, nullptr);
    state.setProperty("hiss", hiss, nullptr);
    state.setProperty("wowFlutter", wowFlutter, nullptr);
}

void Tape::loadState(const juce::ValueTree& state)
{
    if (auto b = state.getProperty("bias"))
        bias = static_cast<float>(*b);
    
    if (auto h = state.getProperty("hiss"))
        hiss = static_cast<float>(*h);
    
    if (auto wf = state.getProperty("wowFlutter"))
        wowFlutter = static_cast<float>(*wf);
    
    modulationDepth = wowFlutter * 0.1f;
    updateLFO();
}
