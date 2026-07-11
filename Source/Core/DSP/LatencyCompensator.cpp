#include "LatencyCompensator.h"

LatencyCompensator::LatencyCompensator()
{
}

LatencyCompensator::~LatencyCompensator()
{
    paths.clear();
}

void LatencyCompensator::prepareToPlay(double newSampleRate, int newBlockSize, int maxLatencySamples)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    this->maxLatencySamples = maxLatencySamples;
    
    outputBuffer.setSize(2, blockSize);
}

void LatencyCompensator::releaseResources()
{
    paths.clear();
    outputBuffer.setSize(0, 0);
}

void LatencyCompensator::addPath(juce::AudioBuffer<float>& buffer, int latencySamples)
{
    Path path;
    path.buffer.makeCopyOf(buffer);
    path.latencySamples = latencySamples;
    path.delayLine.resize(maxLatencySamples * 2, 0.0f); // Stereo
    path.delayLinePosition = 0;
    
    paths.push_back(path);
}

void LatencyCompensator::processAllPaths()
{
    // Find the maximum latency
    int maxLatency = 0;
    for (const auto& path : paths)
    {
        if (path.latencySamples > maxLatency)
            maxLatency = path.latencySamples;
    }
    
    // Process each path with delay compensation
    for (auto& path : paths)
    {
        int delayNeeded = maxLatency - path.latencySamples;
        
        if (delayNeeded > 0)
        {
            addDelayToPath(path, delayNeeded);
        }
    }
}

void LatencyCompensator::getOutput(juce::AudioBuffer<float>& outputBuffer)
{
    // Sum all paths
    outputBuffer.clear();
    
    for (const auto& path : paths)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            auto* outputData = outputBuffer.getWritePointer(channel);
            auto* pathData = path.buffer.getReadPointer(channel);
            
            for (int sample = 0; sample < blockSize; ++sample)
            {
                outputData[sample] += pathData[sample];
            }
        }
    }
}

void LatencyCompensator::reset()
{
    for (auto& path : paths)
    {
        std::fill(path.delayLine.begin(), path.delayLine.end(), 0.0f);
        path.delayLinePosition = 0;
    }
}

void LatencyCompensator::addDelayToPath(Path& path, int delaySamples)
{
    const int numChannels = path.buffer.getNumChannels();
    const int numSamples = path.buffer.getNumSamples();
    
    // Create a temporary buffer for delayed output
    juce::AudioBuffer<float> delayedBuffer(numChannels, numSamples);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* inputData = path.buffer.getReadPointer(channel);
        auto* outputData = delayedBuffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get delayed sample from delay line
            int readPos = (path.delayLinePosition - delaySamples + path.delayLine.size() / numChannels) % (path.delayLine.size() / numChannels);
            float delayedSample = path.delayLine[channel * (path.delayLine.size() / numChannels) + readPos];
            
            // Store current sample in delay line
            path.delayLine[channel * (path.delayLine.size() / numChannels) + path.delayLinePosition] = inputData[sample];
            
            // Output is the delayed sample
            outputData[sample] = delayedSample;
            
            // Update position
            path.delayLinePosition = (path.delayLinePosition + 1) % (path.delayLine.size() / numChannels);
        }
    }
    
    // Copy delayed buffer back to path buffer
    path.buffer.makeCopyOf(delayedBuffer);
}
