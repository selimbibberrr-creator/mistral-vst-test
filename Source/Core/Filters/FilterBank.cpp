#include "FilterBank.h"

FilterBank::FilterBank()
{
}

FilterBank::~FilterBank()
{
}

void FilterBank::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    filterA.prepareToPlay(sampleRate, samplesPerBlock);
    filterB.prepareToPlay(sampleRate, samplesPerBlock);
    
    tempBufferA.setSize(2, samplesPerBlock);
    tempBufferB.setSize(2, samplesPerBlock);
}

void FilterBank::releaseResources()
{
    filterA.releaseResources();
    filterB.releaseResources();
}

void FilterBank::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Make sure temp buffers are the right size
    tempBufferA.setSize(numChannels, numSamples, false, false, true);
    tempBufferB.setSize(numChannels, numSamples, false, false, true);

    switch (routingMode)
    {
        case SERIES:
        {
            // Process through Filter A first
            tempBufferA.makeCopyOf(buffer);
            filterA.processBlock(tempBufferA);
            
            // Then through Filter B
            tempBufferB.makeCopyOf(tempBufferA);
            filterB.processBlock(tempBufferB);
            
            // Copy result back to output
            buffer.makeCopyOf(tempBufferB);
            break;
        }

        case PARALLEL:
        {
            // Process through both filters
            tempBufferA.makeCopyOf(buffer);
            filterA.processBlock(tempBufferA);
            
            tempBufferB.makeCopyOf(buffer);
            filterB.processBlock(tempBufferB);
            
            // Mix the results
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* outData = buffer.getWritePointer(channel);
                auto* aData = tempBufferA.getReadPointer(channel);
                auto* bData = tempBufferB.getReadPointer(channel);

                for (int sample = 0; sample < numSamples; ++sample)
                {
                    outData[sample] = aData[sample] * (1.0f - mixRatio) + bData[sample] * mixRatio;
                }
            }
            break;
        }

        case MID_SIDE:
        {
            // Process Mid with Filter A, Side with Filter B
            tempBufferA.makeCopyOf(buffer);
            filterA.processBlock(tempBufferA);
            
            tempBufferB.makeCopyOf(buffer);
            filterB.processBlock(tempBufferB);
            
            // Convert to Mid/Side, process, then convert back
            for (int sample = 0; sample < numSamples; ++sample)
            {
                // Get left and right
                float left = buffer.getSample(0, sample);
                float right = (numChannels > 1) ? buffer.getSample(1, sample) : 0.0f;
                
                // Convert to Mid/Side
                float mid = (left + right) * 0.5f;
                float side = (left - right) * 0.5f;
                
                // Apply filters
                float filteredMid = tempBufferA.getSample(0, sample);
                float filteredSide = tempBufferB.getSample(0, sample);
                
                // Convert back to stereo
                buffer.setSample(0, sample, filteredMid + filteredSide);
                if (numChannels > 1)
                    buffer.setSample(1, sample, filteredMid - filteredSide);
            }
            break;
        }

        case FEEDBACK:
        {
            // Process through Filter A
            tempBufferA.makeCopyOf(buffer);
            filterA.processBlock(tempBufferA);
            
            // Process through Filter B with feedback from A
            tempBufferB.makeCopyOf(tempBufferA);
            filterB.processBlock(tempBufferB);
            
            // Mix with feedback
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* outData = buffer.getWritePointer(channel);
                auto* aData = tempBufferA.getReadPointer(channel);
                auto* bData = tempBufferB.getReadPointer(channel);

                for (int sample = 0; sample < numSamples; ++sample)
                {
                    // Simple feedback: B output fed back into A input
                    // This is a simplified implementation
                    outData[sample] = bData[sample] + aData[sample] * 0.5f;
                }
            }
            break;
        }
    }
}

void FilterBank::setRoutingMode(RoutingMode mode)
{
    routingMode = mode;
}

void FilterBank::setMixRatio(float ratio)
{
    mixRatio = juce::jlimit(0.0f, 1.0f, ratio);
}

void FilterBank::saveState(juce::ValueTree& state)
{
    auto filterAState = state.getOrCreateChildWithName("FilterA", nullptr);
    filterA.saveState(filterAState);

    auto filterBState = state.getOrCreateChildWithName("FilterB", nullptr);
    filterB.saveState(filterBState);

    state.setProperty("routingMode", static_cast<int>(routingMode), nullptr);
    state.setProperty("mixRatio", mixRatio, nullptr);
}

void FilterBank::loadState(const juce::ValueTree& state)
{
    if (auto aState = state.getChildWithName("FilterA"))
        filterA.loadState(*aState);

    if (auto bState = state.getChildWithName("FilterB"))
        filterB.loadState(*bState);

    if (auto mode = state.getProperty("routingMode"))
        routingMode = static_cast<RoutingMode>(static_cast<int>(*mode));

    if (auto ratio = state.getProperty("mixRatio"))
        mixRatio = static_cast<float>(*ratio);
}
