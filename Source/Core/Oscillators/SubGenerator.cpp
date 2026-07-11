#include "SubGenerator.h"
#include <cmath>

SubGenerator::SubGenerator()
{
    waveformType = SINE; // Sub generator is always a sine wave
}

SubGenerator::~SubGenerator()
{
}

void SubGenerator::processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope)
{
    if (!isNoteOn) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate sample
        float sampleValue = generateSample();
        
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
    }
}

float SubGenerator::generateSample()
{
    // Apply phase offset
    float effectivePhase = phase + phaseOffset;
    if (effectivePhase >= 1.0f) effectivePhase -= 1.0f;
    if (effectivePhase < 0.0f) effectivePhase += 1.0f;

    // Generate pure sine wave
    float sineSample = std::sin(2.0f * juce::MathConstants<float>::pi * effectivePhase);
    
    // Apply saturation (soft clipping)
    if (saturation > 0.0f)
    {
        // Simple soft clipping
        float threshold = 1.0f - saturation;
        if (sineSample > threshold)
            sineSample = threshold + (sineSample - threshold) / (1.0f + saturation * 10.0f);
        else if (sineSample < -threshold)
            sineSample = -threshold - (sineSample + threshold) / (1.0f + saturation * 10.0f);
    }
    
    return sineSample;
}

void SubGenerator::setSaturation(float amount)
{
    saturation = juce::jlimit(0.0f, 1.0f, amount);
}

void SubGenerator::setPhaseOffset(float offset)
{
    phaseOffset = juce::jlimit(0.0f, 1.0f, offset);
}

void SubGenerator::saveState(juce::ValueTree& state)
{
    Oscillator::saveState(state);
    state.setProperty("saturation", saturation, nullptr);
    state.setProperty("phaseOffset", phaseOffset, nullptr);
}

void SubGenerator::loadState(const juce::ValueTree& state)
{
    Oscillator::loadState(state);
    
    if (auto sat = state.getProperty("saturation"))
        saturation = static_cast<float>(*sat);
    
    if (auto offset = state.getProperty("phaseOffset"))
        phaseOffset = static_cast<float>(*offset);
}
