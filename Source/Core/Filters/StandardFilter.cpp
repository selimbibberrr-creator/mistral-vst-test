#include "StandardFilter.h"
#include <cmath>

StandardFilter::StandardFilter()
{
    type = LOWPASS;
    slope = DB12;
}

StandardFilter::~StandardFilter()
{
}

void StandardFilter::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update coefficients if needed
    updateCoefficients();

    // Process each sample
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        FilterState& state = filterState[channel];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSampleInternal(channelData[sample], state);
        }
    }
}

float StandardFilter::processSample(float sample, int channel)
{
    return processSampleInternal(sample, filterState[channel]);
}

void StandardFilter::setType(FilterType newType)
{
    if (type != newType)
    {
        type = newType;
        updateCoefficients();
    }
}

void StandardFilter::setCutoff(float frequency)
{
    if (cutoffFrequency != frequency)
    {
        cutoffFrequency = juce::jlimit(20.0f, 20000.0f, frequency);
        updateCoefficients();
    }
}

void StandardFilter::setResonance(float newResonance)
{
    if (resonance != newResonance)
    {
        resonance = juce::jlimit(0.0f, 1.0f, newResonance);
        updateCoefficients();
    }
}

void StandardFilter::setSlope(Slope newSlope)
{
    if (slope != newSlope)
    {
        slope = newSlope;
        updateCoefficients();
    }
}

void StandardFilter::setKeyTracking(bool enabled)
{
    keyTrackingEnabled = enabled;
}

void StandardFilter::setKeyTrackingAmount(float amount)
{
    keyTrackingAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void StandardFilter::updateCoefficients()
{
    // Calculate effective cutoff with key tracking
    float effectiveCutoff = cutoffFrequency;
    if (keyTrackingEnabled)
    {
        effectiveCutoff += (lastNoteFrequency - 440.0f) * keyTrackingAmount;
        effectiveCutoff = juce::jlimit(20.0f, 20000.0f, effectiveCutoff);
    }

    // Normalize cutoff to 0-1 range
    float omega = 2.0f * juce::MathConstants<float>::pi * effectiveCutoff / static_cast<float>(sampleRate);
    
    // Calculate Q from resonance (0-1 to 0.5-20)
    float q = 1.0f / (1.0f - resonance * 0.95f);
    q = juce::jlimit(0.5f, 20.0f, q);

    // Calculate coefficients based on filter type and slope
    switch (slope)
    {
        case DB12:
            calculate12dBCoefficients(omega, q);
            break;
        case DB24:
            calculate24dBCoefficients(omega, q);
            break;
        case DB48:
            calculate48dBCoefficients(omega, q);
            break;
    }
}

void StandardFilter::calculate12dBCoefficients(float omega, float q)
{
    // 12dB/octave filter coefficients (2-pole)
    float alpha = std::sin(omega) / (2.0f * q);
    float cosOmega = std::cos(omega);

    switch (type)
    {
        case LOWPASS:
        {
            float b0 = (1.0f - cosOmega) / 2.0f;
            float b1 = 1.0f - cosOmega;
            float b2 = (1.0f - cosOmega) / 2.0f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosOmega;
            float a2 = 1.0f - alpha;

            // Normalize
            float invA0 = 1.0f / a0;
            this->b0 = b0 * invA0;
            this->b1 = b1 * invA0;
            this->b2 = b2 * invA0;
            this->a1 = a1 * invA0;
            this->a2 = a2 * invA0;
            this->a0 = 1.0f;
            this->a3 = 0.0f;
            this->a4 = 0.0f;
            this->b3 = 0.0f;
            this->b4 = 0.0f;
            break;
        }

        case HIGHPass:
        {
            float b0 = (1.0f + cosOmega) / 2.0f;
            float b1 = -(1.0f + cosOmega);
            float b2 = (1.0f + cosOmega) / 2.0f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosOmega;
            float a2 = 1.0f - alpha;

            float invA0 = 1.0f / a0;
            this->b0 = b0 * invA0;
            this->b1 = b1 * invA0;
            this->b2 = b2 * invA0;
            this->a1 = a1 * invA0;
            this->a2 = a2 * invA0;
            this->a0 = 1.0f;
            this->a3 = 0.0f;
            this->a4 = 0.0f;
            this->b3 = 0.0f;
            this->b4 = 0.0f;
            break;
        }

        case BANDPASS:
        {
            float b0 = alpha;
            float b1 = 0.0f;
            float b2 = -alpha;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosOmega;
            float a2 = 1.0f - alpha;

            float invA0 = 1.0f / a0;
            this->b0 = b0 * invA0;
            this->b1 = b1 * invA0;
            this->b2 = b2 * invA0;
            this->a1 = a1 * invA0;
            this->a2 = a2 * invA0;
            this->a0 = 1.0f;
            this->a3 = 0.0f;
            this->a4 = 0.0f;
            this->b3 = 0.0f;
            this->b4 = 0.0f;
            break;
        }

        case NOTCH:
        {
            float b0 = 1.0f;
            float b1 = -2.0f * cosOmega;
            float b2 = 1.0f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosOmega;
            float a2 = 1.0f - alpha;

            float invA0 = 1.0f / a0;
            this->b0 = b0 * invA0;
            this->b1 = b1 * invA0;
            this->b2 = b2 * invA0;
            this->a1 = a1 * invA0;
            this->a2 = a2 * invA0;
            this->a0 = 1.0f;
            this->a3 = 0.0f;
            this->a4 = 0.0f;
            this->b3 = 0.0f;
            this->b4 = 0.0f;
            break;
        }
    }
}

void StandardFilter::calculate24dBCoefficients(float omega, float q)
{
    // 24dB/octave filter (4-pole) - cascade two 12dB filters
    float alpha = std::sin(omega) / (2.0f * q);
    float cosOmega = std::cos(omega);

    // First stage coefficients
    float b0_1, b1_1, b2_1, a0_1, a1_1, a2_1;
    
    switch (type)
    {
        case LOWPASS:
        {
            float b0 = (1.0f - cosOmega) / 2.0f;
            float b1 = 1.0f - cosOmega;
            float b2 = (1.0f - cosOmega) / 2.0f;
            a0_1 = 1.0f + alpha;
            a1_1 = -2.0f * cosOmega;
            a2_1 = 1.0f - alpha;

            float invA0 = 1.0f / a0_1;
            b0_1 = b0 * invA0;
            b1_1 = b1 * invA0;
            b2_1 = b2 * invA0;
            a1_1 = a1_1 * invA0;
            a2_1 = a2_1 * invA0;
            break;
        }
        
        case HIGHPass:
        {
            float b0 = (1.0f + cosOmega) / 2.0f;
            float b1 = -(1.0f + cosOmega);
            float b2 = (1.0f + cosOmega) / 2.0f;
            a0_1 = 1.0f + alpha;
            a1_1 = -2.0f * cosOmega;
            a2_1 = 1.0f - alpha;

            float invA0 = 1.0f / a0_1;
            b0_1 = b0 * invA0;
            b1_1 = b1 * invA0;
            b2_1 = b2 * invA0;
            a1_1 = a1_1 * invA0;
            a2_1 = a2_1 * invA0;
            break;
        }
        
        case BANDPASS:
        {
            float b0 = alpha / 2.0f;
            float b1 = 0.0f;
            float b2 = -alpha / 2.0f;
            a0_1 = 1.0f + alpha / 2.0f;
            a1_1 = -2.0f * cosOmega;
            a2_1 = 1.0f - alpha / 2.0f;

            float invA0 = 1.0f / a0_1;
            b0_1 = b0 * invA0;
            b1_1 = b1 * invA0;
            b2_1 = b2 * invA0;
            a1_1 = a1_1 * invA0;
            a2_1 = a2_1 * invA0;
            break;
        }
        
        case NOTCH:
        {
            float b0 = 1.0f;
            float b1 = -2.0f * cosOmega;
            float b2 = 1.0f;
            a0_1 = 1.0f + alpha;
            a1_1 = -2.0f * cosOmega;
            a2_1 = 1.0f - alpha;

            float invA0 = 1.0f / a0_1;
            b0_1 = b0 * invA0;
            b1_1 = b1 * invA0;
            b2_1 = b2 * invA0;
            a1_1 = a1_1 * invA0;
            a2_1 = a2_1 * invA0;
            break;
        }
    }

    // For 24dB, we use the same coefficients for both stages
    // This is a simplified approach - in a real implementation, we'd cascade properly
    this->b0 = b0_1;
    this->b1 = b1_1;
    this->b2 = b2_1;
    this->a1 = a1_1;
    this->a2 = a2_1;
    this->a0 = 1.0f;
    this->a3 = 0.0f;
    this->a4 = 0.0f;
    this->b3 = 0.0f;
    this->b4 = 0.0f;
}

void StandardFilter::calculate48dBCoefficients(float omega, float q)
{
    // 48dB/octave filter - cascade four 12dB filters
    // For simplicity, we'll use the same coefficients as 24dB
    // In a production implementation, this would be more sophisticated
    calculate24dBCoefficients(omega, q);
}

float StandardFilter::processSampleInternal(float sample, FilterState& state)
{
    // Apply the filter based on current coefficients
    // This is a direct form II transposed implementation
    
    // For 12dB (2-pole)
    if (slope == DB12)
    {
        float output = b0 * sample + state.z1;
        state.z1 = b1 * sample - a1 * output + state.z2;
        state.z2 = b2 * sample - a2 * output;
        return output;
    }
    
    // For 24dB (4-pole) - simplified
    else if (slope == DB24)
    {
        // First stage
        float output1 = b0 * sample + state.z1;
        state.z1 = b1 * sample - a1 * output1 + state.z2;
        state.z2 = b2 * sample - a2 * output1;
        
        // Second stage (same coefficients for simplicity)
        float output2 = b0 * output1 + state.z3;
        state.z3 = b1 * output1 - a1 * output2 + state.z4;
        state.z4 = b2 * output1 - a2 * output2;
        
        return output2;
    }
    
    // For 48dB (8-pole) - even more simplified
    else
    {
        // First stage
        float output1 = b0 * sample + state.z1;
        state.z1 = b1 * sample - a1 * output1 + state.z2;
        state.z2 = b2 * sample - a2 * output1;
        
        // Second stage
        float output2 = b0 * output1 + state.z3;
        state.z3 = b1 * output1 - a1 * output2 + state.z4;
        state.z4 = b2 * output1 - a2 * output2;
        
        // Third stage
        float output3 = b0 * output2 + state.z1;
        state.z1 = b1 * output2 - a1 * output3 + state.z2;
        state.z2 = b2 * output2 - a2 * output3;
        
        // Fourth stage
        float output4 = b0 * output3 + state.z3;
        state.z3 = b1 * output3 - a1 * output4 + state.z4;
        state.z4 = b2 * output3 - a2 * output4;
        
        return output4;
    }
}

void StandardFilter::saveState(juce::ValueTree& state)
{
    Filter::saveState(state);
    state.setProperty("keyTracking", keyTrackingEnabled, nullptr);
    state.setProperty("keyTrackingAmount", keyTrackingAmount, nullptr);
}

void StandardFilter::loadState(const juce::ValueTree& state)
{
    Filter::loadState(state);
    
    if (auto kt = state.getProperty("keyTracking"))
        keyTrackingEnabled = static_cast<bool>(*kt);
    
    if (auto kta = state.getProperty("keyTrackingAmount"))
        keyTrackingAmount = static_cast<float>(*kta);
}
