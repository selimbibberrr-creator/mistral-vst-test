#include "BLIT.h"
#include <cmath>

BLIT::BLIT()
{
    harmonics.resize(numHarmonics);
}

BLIT::~BLIT()
{
}

void BLIT::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    updateHarmonics();
}

void BLIT::releaseResources()
{
    harmonics.clear();
}

void BLIT::setFrequency(float newFrequency)
{
    frequency = newFrequency;
    updateHarmonics();
}

void BLIT::setWaveform(int type)
{
    waveformType = juce::jlimit(0, 2, type);
    updateHarmonics();
}

void BLIT::setPulseWidth(float width)
{
    pulseWidth = juce::jlimit(0.01f, 0.99f, width);
    updateHarmonics();
}

float BLIT::generateSample()
{
    float sample = 0.0f;
    
    for (int i = 0; i < numHarmonics; ++i)
    {
        sample += harmonics[i].amplitude * std::sin(harmonics[i].phase);
        harmonics[i].phase += harmonics[i].phaseIncrement;
        
        // Wrap phase
        if (harmonics[i].phase >= 2.0f * juce::MathConstants<float>::pi)
            harmonics[i].phase -= 2.0f * juce::MathConstants<float>::pi;
    }
    
    return sample;
}

void BLIT::generateBlock(float* output, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        output[i] = generateSample();
    }
}

void BLIT::updateHarmonics()
{
    phaseIncrement = 2.0f * juce::MathConstants<float>::pi * frequency / static_cast<float>(sampleRate);
    
    for (int i = 0; i < numHarmonics; ++i)
    {
        int harmonicNumber = i + 1;
        
        // Calculate amplitude based on waveform type
        float amplitude = 0.0f;
        
        switch (waveformType)
        {
            case 0: // Saw wave
                amplitude = 1.0f / harmonicNumber;
                break;
                
            case 1: // Square wave
                if (harmonicNumber % 2 == 1) // Odd harmonics only
                    amplitude = 1.0f / harmonicNumber;
                else
                    amplitude = 0.0f;
                break;
                
            case 2: // Pulse wave
                if (harmonicNumber % 2 == 1) // Odd harmonics only
                    amplitude = (2.0f / (harmonicNumber * juce::MathConstants<float>::pi)) * std::abs(std::sin(harmonicNumber * juce::MathConstants<float>::pi * pulseWidth));
                else
                    amplitude = 0.0f;
                break;
        }
        
        harmonics[i].amplitude = amplitude;
        harmonics[i].phaseIncrement = harmonicNumber * phaseIncrement;
        harmonics[i].phase = 0.0f; // Reset phase
    }
}
