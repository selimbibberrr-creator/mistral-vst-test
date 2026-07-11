#include "StateVariableFilter.h"
#include <cmath>

StateVariableFilter::StateVariableFilter()
{
    type = LOWPASS;
    slope = DB12; // SVF is typically 12dB/octave
}

StateVariableFilter::~StateVariableFilter()
{
}

void StateVariableFilter::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update coefficients if needed
    updateCoefficients();

    // Process each sample
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        SVFState& state = svfState[channel];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSampleInternal(channelData[sample], state);
        }
    }
}

float StateVariableFilter::processSample(float sample, int channel)
{
    return processSampleInternal(sample, svfState[channel]);
}

void StateVariableFilter::setType(FilterType newType)
{
    type = newType;
    
    // Map type to morph value
    switch (type)
    {
        case LOWPASS:    morph = 0.0f; break;
        case BANDPASS:   morph = 0.33f; break;
        case HIGHPass:   morph = 0.66f; break;
        case NOTCH:      morph = 1.0f; break;
    }
    
    updateCoefficients();
}

void StateVariableFilter::setCutoff(float frequency)
{
    cutoffFrequency = juce::jlimit(20.0f, 20000.0f, frequency);
    updateCoefficients();
}

void StateVariableFilter::setResonance(float newResonance)
{
    resonance = juce::jlimit(0.0f, 1.0f, newResonance);
    updateCoefficients();
}

void StateVariableFilter::setSlope(Slope /*newSlope*/)
{
    // SVF is typically 12dB/octave, but we can simulate higher slopes
    // by cascading multiple SVFs
}

void StateVariableFilter::setMorph(float morphValue)
{
    morph = juce::jlimit(0.0f, 1.0f, morphValue);
    updateCoefficients();
}

void StateVariableFilter::setDrive(float amount)
{
    drive = juce::jlimit(0.0f, 1.0f, amount);
}

void StateVariableFilter::updateCoefficients()
{
    // Calculate SVF coefficients
    float wc = 2.0f * juce::MathConstants<float>::pi * cutoffFrequency / static_cast<float>(sampleRate);
    float g = std::tan(wc / 2.0f);
    
    // Calculate resonance (Q)
    float q = 1.0f / (1.0f - resonance * 0.95f);
    q = juce::jlimit(0.5f, 20.0f, q);
    
    // Calculate k (frequency parameter)
    k = 1.0f / q;
    
    // Store g for processing
    this->g = g;
    
    // Calculate output coefficients based on morph value
    // Morph: 0.0=LP, 0.33=BP, 0.66=HP, 1.0=Notch
    if (morph < 0.33f)
    {
        // Morph between LP and BP
        float t = morph / 0.33f;
        a1 = 1.0f - t;
        a2 = t;
        a3 = 0.0f;
    }
    else if (morph < 0.66f)
    {
        // Morph between BP and HP
        float t = (morph - 0.33f) / 0.33f;
        a1 = 0.0f;
        a2 = 1.0f - t;
        a3 = t;
    }
    else
    {
        // Morph between HP and Notch
        float t = (morph - 0.66f) / 0.33f;
        a1 = 0.0f;
        a2 = 0.0f;
        a3 = 1.0f - t;
    }
}

float StateVariableFilter::processSampleInternal(float sample, SVFState& state)
{
    // Apply drive (nonlinear distortion)
    if (drive > 0.0f)
    {
        // Soft clipping
        float threshold = 1.0f - drive * 0.8f;
        if (sample > threshold)
            sample = threshold + (sample - threshold) / (1.0f + drive * 10.0f);
        else if (sample < -threshold)
            sample = -threshold - (sample + threshold) / (1.0f + drive * 10.0f);
    }

    // SVF processing
    float v0 = sample - k * state.ic1eq;
    float v1 = state.ic1eq;
    float v2 = state.ic2eq;
    
    // Update state
    float t0 = std::tan(g / 2.0f);
    float t1 = 1.0f / (1.0f + g * (g + t0));
    float t2 = g * t1;
    float t3 = g * t2;
    
    state.ic1eq += g * (v0 - state.ic1eq + t3 * (v0 - v1));
    state.ic2eq += g * (state.ic1eq - state.ic2eq);
    
    // Calculate outputs
    float lowpass = state.ic2eq;
    float bandpass = state.ic1eq - state.ic2eq;
    float highpass = v0 - state.ic1eq - state.ic2eq;
    float notch = v0 - k * state.ic1eq;
    
    // Morph between outputs
    float output = a1 * lowpass + a2 * bandpass + a3 * highpass;
    
    // For notch, we need special handling
    if (morph >= 0.99f)
        output = notch;
    
    return output;
}

void StateVariableFilter::saveState(juce::ValueTree& state)
{
    Filter::saveState(state);
    state.setProperty("morph", morph, nullptr);
    state.setProperty("drive", drive, nullptr);
}

void StateVariableFilter::loadState(const juce::ValueTree& state)
{
    Filter::loadState(state);
    
    if (auto m = state.getProperty("morph"))
        morph = static_cast<float>(*m);
    
    if (auto d = state.getProperty("drive"))
        drive = static_cast<float>(*d);
    
    updateCoefficients();
}
