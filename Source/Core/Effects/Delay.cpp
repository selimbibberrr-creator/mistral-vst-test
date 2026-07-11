#include "Delay.h"
#include <cmath>

Delay::Delay()
{
}

Delay::~Delay()
{
}

void Delay::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    updateDelayTime();
    delayBuffer.setSize(2, static_cast<int>(sampleRate * 5.0f)); // Max 5 seconds
}

void Delay::releaseResources()
{
}

void Delay::processBlock(juce::AudioBuffer<float>& buffer)
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

void Delay::setTime(float newTime)
{
    time = juce::jmax(0.0f, newTime);
    updateDelayTime();
}

void Delay::setSyncMode(SyncMode newMode)
{
    syncMode = newMode;
    updateDelayTime();
}

void Delay::setFeedback(float newFeedback)
{
    feedback = juce::jlimit(0.0f, 0.99f, newFeedback);
}

void Delay::setFilter(float cutoff)
{
    filterCutoff = juce::jlimit(0.0f, 1.0f, cutoff);
}

void Delay::setBPM(float newBPM)
{
    bpm = juce::jmax(20.0f, newBPM);
    updateDelayTime();
}

void Delay::updateDelayTime()
{
    if (syncMode == MS)
    {
        // Time in milliseconds
        delaySamples = static_cast<int>(time * sampleRate / 1000.0f);
    }
    else
    {
        // Time in beats
        // 60000ms per minute / BPM = ms per beat
        float msPerBeat = 60000.0f / bpm;
        delaySamples = static_cast<int>(time * msPerBeat * sampleRate / 1000.0f);
    }
    
    // Make sure delay buffer is large enough
    if (delaySamples * 2 > delayBuffer.getNumSamples())
    {
        delayBuffer.setSize(2, delaySamples * 2 + 1);
    }
    
    // Reset delay buffer
    delayBuffer.clear();
    delayPosition = 0;
}

float Delay::processSample(float sample, int channel)
{
    // Store current sample in delay buffer
    delayBuffer.setSample(channel, delayPosition, sample);
    
    // Get delayed sample
    int readPos = (delayPosition - delaySamples + delayBuffer.getNumSamples()) % delayBuffer.getNumSamples();
    float delayedSample = delayBuffer.getSample(channel, readPos);
    
    // Apply feedback
    float feedbackSample = delayedSample * feedback;
    
    // Apply lowpass filter to delayed sample
    float filterCoeff = 0.1f + 0.9f * filterCutoff;
    delayedSample = filterState[channel] + filterCoeff * (delayedSample - filterState[channel]);
    filterState[channel] = delayedSample;
    
    // Update delay position
    delayPosition = (delayPosition + 1) % delayBuffer.getNumSamples();
    
    // Mix dry and wet signals
    return sample + delayedSample;
}

void Delay::saveState(juce::ValueTree& state)
{
    state.setProperty("time", time, nullptr);
    state.setProperty("syncMode", static_cast<int>(syncMode), nullptr);
    state.setProperty("feedback", feedback, nullptr);
    state.setProperty("filter", filterCutoff, nullptr);
    state.setProperty("bpm", bpm, nullptr);
}

void Delay::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("time"))
        time = static_cast<float>(*t);
    
    if (auto sm = state.getProperty("syncMode"))
        syncMode = static_cast<SyncMode>(static_cast<int>(*sm));
    
    if (auto fb = state.getProperty("feedback"))
        feedback = static_cast<float>(*fb);
    
    if (auto f = state.getProperty("filter"))
        filterCutoff = static_cast<float>(*f);
    
    if (auto b = state.getProperty("bpm"))
        bpm = static_cast<float>(*b);
    
    updateDelayTime();
}
