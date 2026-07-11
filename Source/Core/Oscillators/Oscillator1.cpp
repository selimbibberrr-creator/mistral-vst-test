#include "Oscillator1.h"
#include <cmath>

Oscillator1::Oscillator1()
{
    waveformType = SINE; // Default to sine wave
}

Oscillator1::~Oscillator1()
{
}

void Oscillator1::processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope)
{
    if (!isNoteOn) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Apply detune
    float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f); // Convert cents to ratio
    float actualFrequency = frequency * detuneFactor;
    float actualPhaseIncrement = actualFrequency / static_cast<float>(sampleRate);

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate sample
        float sampleValue = generateSample() * actualPhaseIncrement;
        
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
        phase += actualPhaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
    }
}

float Oscillator1::generateSample()
{
    // Apply phase offset
    float effectivePhase = phase + phaseOffset;
    if (effectivePhase >= 1.0f) effectivePhase -= 1.0f;
    if (effectivePhase < 0.0f) effectivePhase += 1.0f;

    switch (waveformType)
    {
        case SINE:
            return std::sin(2.0f * juce::MathConstants<float>::pi * effectivePhase);

        case SQUARE:
        {
            // Square wave with PWM
            float saw = effectivePhase * 2.0f - 1.0f;
            float square = (saw > -pulseWidth) ? 1.0f : -1.0f;
            return square;
        }

        case SAW:
        {
            // Saw wave
            float saw = 2.0f * (effectivePhase - 0.5f);
            return saw;
        }

        case TRIANGLE:
        {
            // Triangle wave
            float tri = 4.0f * std::abs(effectivePhase - 0.5f) - 1.0f;
            return tri;
        }

        default:
            return 0.0f;
    }
}

void Oscillator1::setPulseWidth(float width)
{
    pulseWidth = juce::jlimit(0.01f, 0.99f, width); // Prevent 0% or 100% duty cycle
}

void Oscillator1::setDetune(float cents)
{
    detuneCents = juce::jlimit(-50.0f, 50.0f, cents);
}

void Oscillator1::setPhase(float phaseOffset)
{
    this->phaseOffset = juce::jlimit(0.0f, 1.0f, phaseOffset);
}

void Oscillator1::saveState(juce::ValueTree& state)
{
    Oscillator::saveState(state);
    state.setProperty("pulseWidth", pulseWidth, nullptr);
    state.setProperty("detune", detuneCents, nullptr);
    state.setProperty("phase", phaseOffset, nullptr);
}

void Oscillator1::loadState(const juce::ValueTree& state)
{
    Oscillator::loadState(state);
    
    if (auto pw = state.getProperty("pulseWidth"))
        pulseWidth = static_cast<float>(*pw);
    
    if (auto detune = state.getProperty("detune"))
        detuneCents = static_cast<float>(*detune);
    
    if (auto phase = state.getProperty("phase"))
        phaseOffset = static_cast<float>(*phase);
}
