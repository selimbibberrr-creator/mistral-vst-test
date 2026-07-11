#include "Oscillator2.h"
#include <cmath>
#include <random>

Oscillator2::Oscillator2()
{
    waveformType = SINE; // Default to sine wave
}

Oscillator2::~Oscillator2()
{
}

void Oscillator2::processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope)
{
    if (!isNoteOn) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update modulator phase increment for FM
    if (waveformType == FM)
    {
        modulatorPhaseIncrement = modulatorFrequency / static_cast<float>(sampleRate);
    }

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate sample based on waveform type
        float sampleValue = 0.0f;
        
        switch (waveformType)
        {
            case NOISE:
                sampleValue = generateNoiseSample();
                break;
            case WAVETABLE:
                sampleValue = generateWavetableSample(wavetablePosition);
                break;
            case FM:
                sampleValue = generateFMSample();
                break;
            default:
                // Use base class waveform generation
                sampleValue = std::sin(2.0f * juce::MathConstants<float>::pi * phase);
                break;
        }
        
        // Apply envelope
        float envValue = envelope.getNextSample();
        sampleValue *= envValue * currentVelocity;

        // Apply level and pan
        float leftSample = sampleValue * level * (1.0f - pan);
        float rightSample = sampleValue * level * pan;

        // Add to buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.addSample(channel, sample, (channel == 0) ? leftSample : rightSample);
        }

        // Update phase
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
        
        // Update modulator phase for FM
        if (waveformType == FM)
        {
            modulatorPhase += modulatorPhaseIncrement;
            if (modulatorPhase >= 1.0f) modulatorPhase -= 1.0f;
        }
    }
}

float Oscillator2::generateSample()
{
    // This is handled in processBlock based on waveform type
    return 0.0f;
}

float Oscillator2::generateWavetableSample(float position)
{
    // Simple wavetable morphing between sine and square
    float sineSample = std::sin(2.0f * juce::MathConstants<float>::pi * phase);
    float squareSample = (phase < 0.5f) ? 1.0f : -1.0f;
    
    // Linear interpolation
    return juce::jmap(position, 0.0f, 1.0f, sineSample, squareSample);
}

float Oscillator2::generateFMSample()
{
    // FM synthesis: carrier modulated by modulator
    float carrierSample = std::sin(2.0f * juce::MathConstants<float>::pi * phase);
    float modulatorSample = std::sin(2.0f * juce::MathConstants<float>::pi * modulatorPhase);
    
    // Apply FM: carrier frequency modulated by modulator
    float fmPhase = phase + fmIndex * modulatorSample;
    return std::sin(2.0f * juce::MathConstants<float>::pi * fmPhase);
}

float Oscillator2::generateNoiseSample()
{
    // Simple random number generator (will be replaced with better noise)
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    switch (noiseColor)
    {
        case 0: // White noise
            return dist(gen);
            
        case 1: // Pink noise (simplified)
        {
            // Simple pink noise approximation
            float white = dist(gen);
            pinkNoiseState1 = 0.997f * pinkNoiseState1 + 0.029f * white;
            pinkNoiseState2 = 0.985f * pinkNoiseState2 + 0.043f * white;
            pinkNoiseState3 = 0.950f * pinkNoiseState3 + 0.071f * white;
            return pinkNoiseState1 + pinkNoiseState2 + pinkNoiseState3;
        }
            
        case 2: // Brown noise
        {
            // Brownian noise (random walk)
            static float brown = 0.0f;
            brown += dist(gen) * 0.1f;
            brown = juce::jlimit(-1.0f, 1.0f, brown);
            return brown;
        }
        
        default:
            return dist(gen);
    }
}

void Oscillator2::setWavetableType(WavetableType type)
{
    wavetableType = type;
}

void Oscillator2::setWavetablePosition(float position)
{
    wavetablePosition = juce::jlimit(0.0f, 1.0f, position);
}

void Oscillator2::setFMIndex(float index)
{
    fmIndex = juce::jlimit(0.0f, 10.0f, index);
}

void Oscillator2::setModulatorFrequency(float freq)
{
    modulatorFrequency = juce::jmax(0.0f, freq);
    modulatorPhaseIncrement = modulatorFrequency / static_cast<float>(sampleRate);
}

void Oscillator2::setNoiseColor(int color)
{
    noiseColor = juce::jlimit(0, 2, color);
}

void Oscillator2::saveState(juce::ValueTree& state)
{
    Oscillator::saveState(state);
    state.setProperty("wavetableType", static_cast<int>(wavetableType), nullptr);
    state.setProperty("wavetablePosition", wavetablePosition, nullptr);
    state.setProperty("fmIndex", fmIndex, nullptr);
    state.setProperty("modulatorFrequency", modulatorFrequency, nullptr);
    state.setProperty("noiseColor", noiseColor, nullptr);
}

void Oscillator2::loadState(const juce::ValueTree& state)
{
    Oscillator::loadState(state);
    
    if (auto wtType = state.getProperty("wavetableType"))
        wavetableType = static_cast<WavetableType>(static_cast<int>(*wtType));
    
    if (auto wtPos = state.getProperty("wavetablePosition"))
        wavetablePosition = static_cast<float>(*wtPos);
    
    if (auto fmIdx = state.getProperty("fmIndex"))
        fmIndex = static_cast<float>(*fmIdx);
    
    if (auto modFreq = state.getProperty("modulatorFrequency"))
        modulatorFrequency = static_cast<float>(*modFreq);
    
    if (auto nColor = state.getProperty("noiseColor"))
        noiseColor = static_cast<int>(*nColor);
    
    modulatorPhaseIncrement = modulatorFrequency / static_cast<float>(sampleRate);
}
