#include "Mixer.h"

Mixer::Mixer()
{
}

Mixer::~Mixer()
{
}

void Mixer::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    mixBuffer.setSize(2, blockSize);
}

void Mixer::releaseResources()
{
}

void Mixer::processBlock(juce::AudioBuffer<float>& outputBuffer,
                        const juce::AudioBuffer<float>& osc1Buffer,
                        const juce::AudioBuffer<float>& osc2Buffer,
                        const juce::AudioBuffer<float>& noiseBuffer,
                        const juce::AudioBuffer<float>& samplerBuffer,
                        const juce::AudioBuffer<float>& subBuffer)
{
    const int numChannels = outputBuffer.getNumChannels();
    const int numSamples = outputBuffer.getNumSamples();

    // Clear mix buffer
    mixBuffer.clear();

    // Check if any channel is solo
    anySolo = false;
    for (int i = 0; i < 5; ++i)
    {
        if (channels[i].solo)
        {
            anySolo = true;
            break;
        }
    }

    // Mix each channel
    const juce::AudioBuffer<float>* buffers[5] = {
        &osc1Buffer, &osc2Buffer, &noiseBuffer, &samplerBuffer, &subBuffer
    };

    for (int channel = 0; channel < 5; ++channel)
    {
        // Skip muted channels
        if (channels[channel].mute) continue;

        // Skip non-solo channels if any channel is solo
        if (anySolo && !channels[channel].solo) continue;

        const juce::AudioBuffer<float>& inputBuffer = *buffers[channel];
        float level = channels[channel].level;
        float pan = channels[channel].pan;

        // Process each sample
        for (int c = 0; c < numChannels; ++c)
        {
            auto* mixData = mixBuffer.getWritePointer(c);
            auto* inputData = inputBuffer.getReadPointer(c);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                float inputSample = inputData[sample];
                
                // Apply pan law (equal power)
                float leftGain = std::cos(pan * juce::MathConstants<float>::pi / 2.0f);
                float rightGain = std::sin(pan * juce::MathConstants<float>::pi / 2.0f);
                
                if (c == 0) // Left channel
                {
                    mixData[sample] += inputSample * level * leftGain;
                }
                else if (c == 1) // Right channel
                {
                    mixData[sample] += inputSample * level * rightGain;
                }
            }
        }
    }

    // Apply master level and pan
    for (int c = 0; c < numChannels; ++c)
    {
        auto* outputData = outputBuffer.getWritePointer(c);
        auto* mixData = mixBuffer.getReadPointer(c);

        float masterGain = (c == 0) ? (1.0f - masterPan) : masterPan;
        masterGain *= masterLevel;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            outputData[sample] = mixData[sample] * masterGain;
        }
    }
}

void Mixer::setLevel(int channel, float level)
{
    if (channel >= 0 && channel < 5)
        channels[channel].level = juce::jlimit(0.0f, 1.0f, level);
}

void Mixer::setPan(int channel, float pan)
{
    if (channel >= 0 && channel < 5)
        channels[channel].pan = juce::jlimit(0.0f, 1.0f, pan);
}

void Mixer::setMute(int channel, bool mute)
{
    if (channel >= 0 && channel < 5)
        channels[channel].mute = mute;
}

void Mixer::setSolo(int channel, bool solo)
{
    if (channel >= 0 && channel < 5)
        channels[channel].solo = solo;
}

float Mixer::getLevel(int channel) const
{
    if (channel >= 0 && channel < 5)
        return channels[channel].level;
    return 1.0f;
}

float Mixer::getPan(int channel) const
{
    if (channel >= 0 && channel < 5)
        return channels[channel].pan;
    return 0.5f;
}

bool Mixer::getMute(int channel) const
{
    if (channel >= 0 && channel < 5)
        return channels[channel].mute;
    return false;
}

bool Mixer::getSolo(int channel) const
{
    if (channel >= 0 && channel < 5)
        return channels[channel].solo;
    return false;
}

void Mixer::setMasterLevel(float level)
{
    masterLevel = juce::jlimit(0.0f, 1.0f, level);
}

void Mixer::setMasterPan(float pan)
{
    masterPan = juce::jlimit(0.0f, 1.0f, pan);
}

void Mixer::saveState(juce::ValueTree& state)
{
    for (int i = 0; i < 5; ++i)
    {
        auto channelState = state.getOrCreateChildWithName("Channel" + juce::String(i), nullptr);
        channelState.setProperty("level", channels[i].level, nullptr);
        channelState.setProperty("pan", channels[i].pan, nullptr);
        channelState.setProperty("mute", channels[i].mute, nullptr);
        channelState.setProperty("solo", channels[i].solo, nullptr);
    }
    
    state.setProperty("masterLevel", masterLevel, nullptr);
    state.setProperty("masterPan", masterPan, nullptr);
}

void Mixer::loadState(const juce::ValueTree& state)
{
    for (int i = 0; i < 5; ++i)
    {
        if (auto channelState = state.getChildWithName("Channel" + juce::String(i)))
        {
            if (auto level = channelState->getProperty("level"))
                channels[i].level = static_cast<float>(*level);
            
            if (auto pan = channelState->getProperty("pan"))
                channels[i].pan = static_cast<float>(*pan);
            
            if (auto mute = channelState->getProperty("mute"))
                channels[i].mute = static_cast<bool>(*mute);
            
            if (auto solo = channelState->getProperty("solo"))
                channels[i].solo = static_cast<bool>(*solo);
        }
    }
    
    if (auto ml = state.getProperty("masterLevel"))
        masterLevel = static_cast<float>(*ml);
    
    if (auto mp = state.getProperty("masterPan"))
        masterPan = static_cast<float>(*mp);
}
