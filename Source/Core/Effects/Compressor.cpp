#include "Compressor.h"
#include <cmath>

Compressor::Compressor()
{
}

Compressor::~Compressor()
{
}

void Compressor::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    updateCoefficients();
}

void Compressor::releaseResources()
{
}

void Compressor::processBlock(juce::AudioBuffer<float>& buffer)
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

void Compressor::setType(CompressorType newType)
{
    type = newType;
}

void Compressor::setThreshold(float newThreshold)
{
    threshold = newThreshold;
}

void Compressor::setRatio(float newRatio)
{
    ratio = juce::jmax(1.0f, newRatio);
}

void Compressor::setAttack(float newAttack)
{
    attack = juce::jmax(0.0f, newAttack);
    updateCoefficients();
}

void Compressor::setRelease(float newRelease)
{
    release = juce::jmax(0.0f, newRelease);
    updateCoefficients();
}

void Compressor::setMakeupGain(float newGain)
{
    makeupGain = newGain;
}

void Compressor::setSidechainEnabled(bool enabled)
{
    sidechainEnabled = enabled;
}

void Compressor::setSidechainInput(const juce::AudioBuffer<float>& sidechainBuffer)
{
    this->sidechainBuffer = &sidechainBuffer;
}

void Compressor::updateCoefficients()
{
    float samplesPerMs = static_cast<float>(sampleRate) / 1000.0f;
    
    // Attack coefficient (exponential approach)
    if (attack > 0.0f)
        attackCoeff = 1.0f - std::exp(-1.0f / (attack * samplesPerMs));
    else
        attackCoeff = 1.0f; // Instant attack
    
    // Release coefficient (exponential decay)
    if (release > 0.0f)
        releaseCoeff = 1.0f - std::exp(-1.0f / (release * samplesPerMs));
    else
        releaseCoeff = 1.0f; // Instant release
}

float Compressor::processSample(float sample, int channel)
{
    // Get sidechain input or use the main signal
    float sidechainSample = sample;
    if (sidechainEnabled && sidechainBuffer != nullptr && 
        channel < sidechainBuffer->getNumChannels())
    {
        int sidechainSample = static_cast<int>(sample * sidechainBuffer->getNumSamples() / blockSize);
        sidechainSample = juce::jlimit(0, sidechainBuffer->getNumSamples() - 1, sidechainSample);
        sidechainSample = sidechainBuffer->getSample(channel, sidechainSample);
    }
    
    // Calculate absolute value for envelope detection
    float absSample = std::abs(sidechainSample);
    
    // Convert to dB
    float sampleDB = 20.0f * std::log10(absSample + 0.00001f);
    
    // Calculate gain reduction
    float gainReduction = 0.0f;
    if (sampleDB > threshold)
    {
        float overDB = sampleDB - threshold;
        gainReduction = overDB * (1.0f - 1.0f / ratio);
    }
    
    // Convert gain reduction to linear scale
    float linearGainReduction = juce::Decibels::decibelsToGain(-gainReduction);
    
    // Apply envelope (simplified - in real implementation, this would be smoothed)
    float targetEnvelope = linearGainReduction;
    
    // Simple envelope follower (this is a simplified version)
    if (targetEnvelope < envelope)
        envelope = envelope * (1.0f - attackCoeff) + targetEnvelope * attackCoeff;
    else
        envelope = envelope * (1.0f - releaseCoeff) + targetEnvelope * releaseCoeff;
    
    // Apply makeup gain
    float makeupFactor = juce::Decibels::decibelsToGain(makeupGain);
    
    // Apply compression
    return sample * envelope * makeupFactor;
}

void Compressor::saveState(juce::ValueTree& state)
{
    state.setProperty("type", static_cast<int>(type), nullptr);
    state.setProperty("threshold", threshold, nullptr);
    state.setProperty("ratio", ratio, nullptr);
    state.setProperty("attack", attack, nullptr);
    state.setProperty("release", release, nullptr);
    state.setProperty("makeupGain", makeupGain, nullptr);
    state.setProperty("sidechainEnabled", sidechainEnabled, nullptr);
}

void Compressor::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("type"))
        type = static_cast<CompressorType>(static_cast<int>(*t));
    
    if (auto th = state.getProperty("threshold"))
        threshold = static_cast<float>(*th);
    
    if (auto r = state.getProperty("ratio"))
        ratio = static_cast<float>(*r);
    
    if (auto a = state.getProperty("attack"))
        attack = static_cast<float>(*a);
    
    if (auto r = state.getProperty("release"))
        release = static_cast<float>(*r);
    
    if (auto mg = state.getProperty("makeupGain"))
        makeupGain = static_cast<float>(*mg);
    
    if (auto sc = state.getProperty("sidechainEnabled"))
        sidechainEnabled = static_cast<bool>(*sc);
    
    updateCoefficients();
}
