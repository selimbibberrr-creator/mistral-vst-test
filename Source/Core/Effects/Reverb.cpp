#include "Reverb.h"
#include <cmath>
#include <random>

Reverb::Reverb()
{
}

Reverb::~Reverb()
{
}

void Reverb::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    updateDelayLines();
    preDelayBuffer.setSize(2, static_cast<int>(0.5f * sampleRate / 1000.0f) + 1); // Max 500ms
}

void Reverb::releaseResources()
{
    for (auto& line : delayLines)
        line.clear();
    for (auto& line : allpassLines)
        line.clear();
}

void Reverb::processBlock(juce::AudioBuffer<float>& buffer)
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

void Reverb::setType(ReverbType newType)
{
    type = newType;
    updateDelayLines();
}

void Reverb::setDecay(float newDecay)
{
    decay = juce::jlimit(0.1f, 10.0f, newDecay);
    updateDelayLines();
}

void Reverb::setPreDelay(float newPreDelay)
{
    preDelay = juce::jlimit(0.0f, 500.0f, newPreDelay);
    preDelayBuffer.setSize(2, static_cast<int>(preDelay * sampleRate / 1000.0f) + 1);
}

void Reverb::setSize(float newSize)
{
    size = juce::jlimit(0.0f, 1.0f, newSize);
    updateDelayLines();
}

void Reverb::setDamping(float newDamping)
{
    damping = juce::jlimit(0.0f, 1.0f, newDamping);
}

void Reverb::setMix(float newMix)
{
    mix = juce::jlimit(0.0f, 1.0f, newMix);
}

void Reverb::updateDelayLines()
{
    // Calculate delay line sizes based on parameters
    // These are simplified - in a real implementation, they would be more sophisticated
    
    int maxDelaySamples = static_cast<int>(sampleRate * 10.0); // 10 seconds max
    
    // Clear existing lines
    for (auto& line : delayLines)
        line.clear();
    for (auto& line : allpassLines)
        line.clear();
    
    // Calculate sizes based on reverb type
    switch (type)
    {
        case HALL:
        {
            // Hall reverb has longer delay times
            int size1 = static_cast<int>(sampleRate * (0.1f + 0.9f * size));
            int size2 = static_cast<int>(sampleRate * (0.15f + 0.85f * size));
            int size3 = static_cast<int>(sampleRate * (0.2f + 0.8f * size));
            int size4 = static_cast<int>(sampleRate * (0.25f + 0.75f * size));
            
            delayLines[0].resize(size1, 0.0f);
            delayLines[1].resize(size2, 0.0f);
            delayLines[2].resize(size3, 0.0f);
            delayLines[3].resize(size4, 0.0f);
            
            // Allpass lines
            allpassLines[0].resize(static_cast<int>(sampleRate * 0.05f), 0.0f);
            allpassLines[1].resize(static_cast<int>(sampleRate * 0.03f), 0.0f);
            break;
        }
        
        case ROOM:
        {
            // Room reverb has shorter delay times
            int size1 = static_cast<int>(sampleRate * (0.05f + 0.45f * size));
            int size2 = static_cast<int>(sampleRate * (0.08f + 0.42f * size));
            int size3 = static_cast<int>(sampleRate * (0.1f + 0.4f * size));
            int size4 = static_cast<int>(sampleRate * (0.12f + 0.38f * size));
            
            delayLines[0].resize(size1, 0.0f);
            delayLines[1].resize(size2, 0.0f);
            delayLines[2].resize(size3, 0.0f);
            delayLines[3].resize(size4, 0.0f);
            
            // Allpass lines
            allpassLines[0].resize(static_cast<int>(sampleRate * 0.02f), 0.0f);
            allpassLines[1].resize(static_cast<int>(sampleRate * 0.015f), 0.0f);
            break;
        }
        
        case SPRING:
        {
            // Spring reverb has very short delay times
            int size1 = static_cast<int>(sampleRate * (0.01f + 0.09f * size));
            int size2 = static_cast<int>(sampleRate * (0.02f + 0.08f * size));
            int size3 = static_cast<int>(sampleRate * (0.03f + 0.07f * size));
            int size4 = static_cast<int>(sampleRate * (0.04f + 0.06f * size));
            
            delayLines[0].resize(size1, 0.0f);
            delayLines[1].resize(size2, 0.0f);
            delayLines[2].resize(size3, 0.0f);
            delayLines[3].resize(size4, 0.0f);
            
            // Allpass lines
            allpassLines[0].resize(static_cast<int>(sampleRate * 0.005f), 0.0f);
            allpassLines[1].resize(static_cast<int>(sampleRate * 0.004f), 0.0f);
            break;
        }
    }
    
    // Reset positions
    for (int i = 0; i < 4; ++i)
        delayLinePositions[i] = 0;
    for (int i = 0; i < 2; ++i)
        allpassPositions[i] = 0;
}

float Reverb::processSample(float sample, int channel)
{
    // Pre-delay
    int preDelaySamples = static_cast<int>(preDelay * sampleRate / 1000.0f);
    if (preDelaySamples > 0)
    {
        // Store in pre-delay buffer
        preDelayBuffer.setSample(channel, preDelayPosition, sample);
        preDelayPosition = (preDelayPosition + 1) % preDelayBuffer.getNumSamples();
        
        // Get delayed sample
        int readPos = (preDelayPosition - preDelaySamples + preDelayBuffer.getNumSamples()) % preDelayBuffer.getNumSamples();
        sample = preDelayBuffer.getSample(channel, readPos);
    }
    
    // Calculate feedback based on decay
    float feedback = std::pow(0.001f, 1.0f / (decay * sampleRate / 44100.0f));
    
    // Apply damping (lowpass filter)
    float dampingCoeff = 0.1f + 0.9f * damping;
    
    // Process through delay lines and allpass filters
    // This is a simplified Schroeder reverb implementation
    
    // Left channel uses delay lines 0 and 2
    // Right channel uses delay lines 1 and 3
    int dlIndex1 = channel * 2;
    int dlIndex2 = channel * 2 + 1;
    
    // Get samples from delay lines
    int pos1 = delayLinePositions[dlIndex1];
    int pos2 = delayLinePositions[dlIndex2];
    
    float delayed1 = delayLines[dlIndex1][pos1];
    float delayed2 = delayLines[dlIndex2][pos2];
    
    // Allpass processing
    float allpass1 = allpassLines[0][allpassPositions[0]];
    float allpass2 = allpassLines[1][allpassPositions[1]];
    
    // Simple allpass implementation
    float allpassOutput1 = -allpass1 + 0.5f * sample;
    float allpassOutput2 = -allpass2 + 0.5f * delayed1;
    
    // Update allpass lines
    allpassLines[0][allpassPositions[0]] = allpassOutput1;
    allpassLines[1][allpassPositions[1]] = allpassOutput2;
    allpassPositions[0] = (allpassPositions[0] + 1) % allpassLines[0].size();
    allpassPositions[1] = (allpassPositions[1] + 1) % allpassLines[1].size();
    
    // Mix all signals
    float reverbOutput = delayed1 + delayed2 + allpassOutput1 + allpassOutput2;
    
    // Apply damping (simple lowpass)
    reverbOutput = dampingFilterState[channel][0] + dampingCoeff * (reverbOutput - dampingFilterState[channel][0]);
    dampingFilterState[channel][0] = reverbOutput;
    
    // Update delay lines
    delayLines[dlIndex1][pos1] = sample + delayed2 * feedback;
    delayLines[dlIndex2][pos2] = delayed1 + delayed2 * feedback;
    
    delayLinePositions[dlIndex1] = (pos1 + 1) % delayLines[dlIndex1].size();
    delayLinePositions[dlIndex2] = (pos2 + 1) % delayLines[dlIndex2].size();
    
    // Mix dry and wet signals
    return sample * (1.0f - mix) + reverbOutput * mix * 0.3f;
}

float Reverb::random() const
{
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(gen);
}

void Reverb::saveState(juce::ValueTree& state)
{
    state.setProperty("type", static_cast<int>(type), nullptr);
    state.setProperty("decay", decay, nullptr);
    state.setProperty("preDelay", preDelay, nullptr);
    state.setProperty("size", size, nullptr);
    state.setProperty("damping", damping, nullptr);
    state.setProperty("mix", mix, nullptr);
}

void Reverb::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("type"))
        type = static_cast<ReverbType>(static_cast<int>(*t));
    
    if (auto d = state.getProperty("decay"))
        decay = static_cast<float>(*d);
    
    if (auto pd = state.getProperty("preDelay"))
        preDelay = static_cast<float>(*pd);
    
    if (auto s = state.getProperty("size"))
        size = static_cast<float>(*s);
    
    if (auto d = state.getProperty("damping"))
        damping = static_cast<float>(*d);
    
    if (auto m = state.getProperty("mix"))
        mix = static_cast<float>(*m);
    
    updateDelayLines();
}
