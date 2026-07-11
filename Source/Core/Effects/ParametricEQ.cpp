#include "ParametricEQ.h"
#include <cmath>

ParametricEQ::ParametricEQ()
{
    // Initialize default bands
    bands[0] = {BELL, 100.0f, 0.7f, 0.0f, true};
    bands[1] = {BELL, 500.0f, 0.7f, 0.0f, true};
    bands[2] = {BELL, 2000.0f, 0.7f, 0.0f, true};
    bands[3] = {BELL, 5000.0f, 0.7f, 0.0f, true};
    bands[4] = {LOW_SHELF, 80.0f, 0.7f, 0.0f, true};
    bands[5] = {HIGH_SHELF, 10000.0f, 0.7f, 0.0f, true};
}

ParametricEQ::~ParametricEQ()
{
}

void ParametricEQ::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    
    // Update all band coefficients
    for (int i = 0; i < 6; ++i)
    {
        updateBandCoefficients(i);
    }
    
    // Initialize oversampling buffers
    oversampledBuffer.setSize(2, blockSize * 2);
    downsampledBuffer.setSize(2, blockSize);
}

void ParametricEQ::releaseResources()
{
}

void ParametricEQ::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSample(channelData[sample], channel);
        }
    }
}

void ParametricEQ::setBand(int bandIndex, BandType type, float frequency, float q, float gain)
{
    if (bandIndex >= 0 && bandIndex < 6)
    {
        bands[bandIndex].type = type;
        bands[bandIndex].frequency = juce::jlimit(20.0f, 20000.0f, frequency);
        bands[bandIndex].q = juce::jlimit(0.1f, 10.0f, q);
        bands[bandIndex].gain = juce::jlimit(-24.0f, 24.0f, gain);
        updateBandCoefficients(bandIndex);
    }
}

ParametricEQ::EQBand& ParametricEQ::getBand(int bandIndex)
{
    return bands[bandIndex];
}

const ParametricEQ::EQBand& ParametricEQ::getBand(int bandIndex) const
{
    return bands[bandIndex];
}

void ParametricEQ::enableBand(int bandIndex, bool enabled)
{
    if (bandIndex >= 0 && bandIndex < 6)
    {
        bands[bandIndex].enabled = enabled;
    }
}

void ParametricEQ::setOversampling(int factor)
{
    oversampling = juce::jlimit(1, 2, factor);
}

void ParametricEQ::setLinearPhase(bool enabled)
{
    linearPhase = enabled;
}

void ParametricEQ::updateBandCoefficients(int bandIndex)
{
    if (bandIndex < 0 || bandIndex >= 6) return;

    EQBand& band = bands[bandIndex];
    
    // Calculate coefficients based on band type
    float omega = 2.0f * juce::MathConstants<float>::pi * band.frequency / static_cast<float>(sampleRate);
    float sn = std::sin(omega);
    float cs = std::cos(omega);
    float alpha = sn / (2.0f * band.q);
    
    // Convert gain to linear scale
    float A = juce::Decibels::decibelsToGain(band.gain);
    
    // Reset filter states when coefficients change
    for (int channel = 0; channel < 2; ++channel)
    {
        filterStates[bandIndex][channel].z1 = 0.0f;
        filterStates[bandIndex][channel].z2 = 0.0f;
    }
    
    // Note: In a real implementation, we would store the coefficients
    // For simplicity, we'll calculate them on the fly in processSample
}

float ParametricEQ::processSample(float sample, int channel)
{
    float result = sample;
    
    // Process through each band
    for (int bandIndex = 0; bandIndex < 6; ++bandIndex)
    {
        EQBand& band = bands[bandIndex];
        if (!band.enabled) continue;
        
        // Calculate coefficients on the fly (for simplicity)
        float omega = 2.0f * juce::MathConstants<float>::pi * band.frequency / static_cast<float>(sampleRate);
        float sn = std::sin(omega);
        float cs = std::cos(omega);
        float alpha = sn / (2.0f * band.q);
        float A = juce::Decibels::decibelsToGain(band.gain);
        
        FilterState& state = filterStates[bandIndex][channel];
        
        switch (band.type)
        {
            case BELL:
            {
                // Bell filter (peaking EQ)
                float b0 = 1.0f + alpha * A;
                float b1 = -2.0f * cs;
                float b2 = 1.0f - alpha * A;
                float a0 = 1.0f + alpha / A;
                float a1 = -2.0f * cs;
                float a2 = 1.0f - alpha / A;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                // Direct form II transposed
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
            
            case NOTCH:
            {
                // Notch filter
                float b0 = 1.0f;
                float b1 = -2.0f * cs;
                float b2 = 1.0f;
                float a0 = 1.0f + alpha;
                float a1 = -2.0f * cs;
                float a2 = 1.0f - alpha;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
            
            case LOW_SHELF:
            {
                // Low shelf
                float b0 = A * ((A + 1.0f) - (A - 1.0f) * cs + 2.0f * std::sqrt(A) * alpha);
                float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cs);
                float b2 = A * ((A + 1.0f) - (A - 1.0f) * cs - 2.0f * std::sqrt(A) * alpha);
                float a0 = (A + 1.0f) + (A - 1.0f) * cs + 2.0f * std::sqrt(A) * alpha;
                float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cs);
                float a2 = (A + 1.0f) + (A - 1.0f) * cs - 2.0f * std::sqrt(A) * alpha;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
            
            case HIGH_SHELF:
            {
                // High shelf
                float b0 = A * ((A + 1.0f) + (A - 1.0f) * cs + 2.0f * std::sqrt(A) * alpha);
                float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cs);
                float b2 = A * ((A + 1.0f) + (A - 1.0f) * cs - 2.0f * std::sqrt(A) * alpha);
                float a0 = (A + 1.0f) - (A - 1.0f) * cs + 2.0f * std::sqrt(A) * alpha;
                float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cs);
                float a2 = (A + 1.0f) - (A - 1.0f) * cs - 2.0f * std::sqrt(A) * alpha;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
            
            case LOW_PASS:
            {
                // Low pass
                float b0 = (1.0f - cs) / 2.0f;
                float b1 = 1.0f - cs;
                float b2 = (1.0f - cs) / 2.0f;
                float a0 = 1.0f + alpha;
                float a1 = -2.0f * cs;
                float a2 = 1.0f - alpha;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
            
            case HIGH_PASS:
            {
                // High pass
                float b0 = (1.0f + cs) / 2.0f;
                float b1 = -(1.0f + cs);
                float b2 = (1.0f + cs) / 2.0f;
                float a0 = 1.0f + alpha;
                float a1 = -2.0f * cs;
                float a2 = 1.0f - alpha;
                
                float invA0 = 1.0f / a0;
                b0 *= invA0;
                b1 *= invA0;
                b2 *= invA0;
                a1 *= invA0;
                a2 *= invA0;
                
                float output = b0 * result + state.z1;
                state.z1 = b1 * result - a1 * output + state.z2;
                state.z2 = b2 * result - a2 * output;
                result = output;
                break;
            }
        }
    }
    
    return result;
}

void ParametricEQ::saveState(juce::ValueTree& state)
{
    for (int i = 0; i < 6; ++i)
    {
        auto bandState = state.getOrCreateChildWithName("Band" + juce::String(i), nullptr);
        bandState.setProperty("type", static_cast<int>(bands[i].type), nullptr);
        bandState.setProperty("frequency", bands[i].frequency, nullptr);
        bandState.setProperty("q", bands[i].q, nullptr);
        bandState.setProperty("gain", bands[i].gain, nullptr);
        bandState.setProperty("enabled", bands[i].enabled, nullptr);
    }
    
    state.setProperty("oversampling", oversampling, nullptr);
    state.setProperty("linearPhase", linearPhase, nullptr);
}

void ParametricEQ::loadState(const juce::ValueTree& state)
{
    for (int i = 0; i < 6; ++i)
    {
        if (auto bandState = state.getChildWithName("Band" + juce::String(i)))
        {
            if (auto t = bandState->getProperty("type"))
                bands[i].type = static_cast<BandType>(static_cast<int>(*t));
            
            if (auto f = bandState->getProperty("frequency"))
                bands[i].frequency = static_cast<float>(*f);
            
            if (auto q = bandState->getProperty("q"))
                bands[i].q = static_cast<float>(*q);
            
            if (auto g = bandState->getProperty("gain"))
                bands[i].gain = static_cast<float>(*g);
            
            if (auto e = bandState->getProperty("enabled"))
                bands[i].enabled = static_cast<bool>(*e);
            
            updateBandCoefficients(i);
        }
    }
    
    if (auto os = state.getProperty("oversampling"))
        oversampling = static_cast<int>(*os);
    
    if (auto lp = state.getProperty("linearPhase"))
        linearPhase = static_cast<bool>(*lp);
}
