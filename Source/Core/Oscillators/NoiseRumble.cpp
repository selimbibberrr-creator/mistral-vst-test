#include "NoiseRumble.h"
#include <cmath>
#include <random>

NoiseRumble::NoiseRumble()
{
    waveformType = NOISE;
}

NoiseRumble::~NoiseRumble()
{
}

void NoiseRumble::processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope)
{
    if (!isNoteOn) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update decay increment
    float samplesPerMs = static_cast<float>(sampleRate) / 1000.0f;
    decayIncrement = 1.0f / (decayTime * samplesPerMs);

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate noise sample
        float noise = generateNoiseSample();
        
        // Apply filter
        float filteredNoise = applyFilter(noise);
        
        // Apply envelope
        float envValue = envelope.getNextSample();
        
        // Apply decay
        decayValue -= decayIncrement;
        if (decayValue < 0.0f) decayValue = 0.0f;
        
        // Combine envelope and decay
        float combinedEnv = envValue * decayValue;
        
        // Apply level and pan
        float leftSample = filteredNoise * level * combinedEnv * (1.0f - pan);
        float rightSample = filteredNoise * level * combinedEnv * pan;

        // Add to buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.addSample(channel, sample, (channel == 0) ? leftSample : rightSample);
        }
    }
}

float NoiseRumble::generateNoiseSample()
{
    // Simple random number generator
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    // For now, just white noise
    return dist(gen);
}

float NoiseRumble::applyFilter(float sample)
{
    // Simple state-variable filter implementation
    const float pi = juce::MathConstants<float>::pi;
    const float omega = 2.0f * pi * cutoffFrequency / static_cast<float>(sampleRate);
    const float q = 1.0f - omega / (2.0f * resonance + 1.0f);
    
    switch (filterType)
    {
        case LOWPASS:
        {
            // Simple lowpass filter
            filterState1 = filterState1 + omega * (sample - filterState1);
            return filterState1;
        }
        
        case HIGHPass:
        {
            // Simple highpass filter
            filterState1 = filterState1 + omega * (sample - filterState1);
            return sample - filterState1;
        }
        
        case BANDPASS:
        {
            // Simple bandpass filter
            filterState1 = filterState1 + omega * (sample - filterState1);
            filterState2 = filterState2 + omega * (filterState1 - filterState2);
            return filterState2;
        }
        
        case NOTCH:
        {
            // Simple notch filter
            filterState1 = filterState1 + omega * (sample - filterState1);
            filterState2 = filterState2 + omega * (filterState1 - filterState2);
            return sample - filterState2;
        }
        
        default:
            return sample;
    }
}

void NoiseRumble::setFilterType(FilterType type)
{
    filterType = type;
}

void NoiseRumble::setCutoffFrequency(float frequency)
{
    cutoffFrequency = juce::jmax(20.0f, juce::jmin(20000.0f, frequency));
}

void NoiseRumble::setResonance(float newResonance)
{
    resonance = juce::jlimit(0.0f, 1.0f, newResonance);
}

void NoiseRumble::setDecayTime(float timeMs)
{
    decayTime = juce::jmax(0.0f, timeMs);
}

void NoiseRumble::saveState(juce::ValueTree& state)
{
    Oscillator::saveState(state);
    state.setProperty("filterType", static_cast<int>(filterType), nullptr);
    state.setProperty("cutoffFrequency", cutoffFrequency, nullptr);
    state.setProperty("resonance", resonance, nullptr);
    state.setProperty("decayTime", decayTime, nullptr);
}

void NoiseRumble::loadState(const juce::ValueTree& state)
{
    Oscillator::loadState(state);
    
    if (auto fType = state.getProperty("filterType"))
        filterType = static_cast<FilterType>(static_cast<int>(*fType));
    
    if (auto cutoff = state.getProperty("cutoffFrequency"))
        cutoffFrequency = static_cast<float>(*cutoff);
    
    if (auto res = state.getProperty("resonance"))
        resonance = static_cast<float>(*res);
    
    if (auto decay = state.getProperty("decayTime"))
        decayTime = static_cast<float>(*decay);
}
