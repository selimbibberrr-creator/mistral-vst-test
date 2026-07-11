#include "Oversampling.h"
#include <vector>

Oversampler::Oversampler()
{
}

Oversampler::~Oversampler()
{
}

void Oversampler::prepareToPlay(double newSampleRate, int newBlockSize, int factor)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    oversamplingFactor = juce::jlimit(1, 4, factor);
    
    // Allocate buffers
    upsampledBuffer.setSize(2, blockSize * oversamplingFactor);
    downsampledBuffer.setSize(2, blockSize);
}

void Oversampler::releaseResources()
{
    upsampledBuffer.setSize(0, 0);
    downsampledBuffer.setSize(0, 0);
}

void Oversampler::setFactor(int factor)
{
    oversamplingFactor = juce::jlimit(1, 4, factor);
    upsampledBuffer.setSize(2, blockSize * oversamplingFactor);
}

void Oversampler::processBlock(juce::AudioBuffer<float>& buffer, 
                              std::function<void(juce::AudioBuffer<float>&)> processFunction)
{
    if (oversamplingFactor <= 1)
    {
        // No oversampling, just process directly
        processFunction(buffer);
        return;
    }

    // Upsample
    upsample(buffer, upsampledBuffer);

    // Process at higher sample rate
    processFunction(upsampledBuffer);

    // Downsample back
    downsample(upsampledBuffer, downsampledBuffer);

    // Copy back to output
    buffer.makeCopyOf(downsampledBuffer);
}

void Oversampler::upsample(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output)
{
    const int numChannels = input.getNumChannels();
    const int inputSamples = input.getNumSamples();
    const int outputSamples = output.getNumSamples();

    // Simple zero-order hold upsampling (for now)
    // In a production implementation, use polyphase filtering
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* inputData = input.getReadPointer(channel);
        auto* outputData = output.getWritePointer(channel);

        for (int i = 0; i < outputSamples; ++i)
        {
            // Map output sample to input sample
            int inputIndex = i / oversamplingFactor;
            inputIndex = juce::jmin(inputIndex, inputSamples - 1);
            outputData[i] = inputData[inputIndex];
        }
    }
}

void Oversampler::downsample(const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output)
{
    const int numChannels = input.getNumChannels();
    const int inputSamples = input.getNumSamples();
    const int outputSamples = output.getNumSamples();

    // Simple averaging downsampling
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* inputData = input.getReadPointer(channel);
        auto* outputData = output.getWritePointer(channel);

        for (int i = 0; i < outputSamples; ++i)
        {
            float sum = 0.0f;
            int startSample = i * oversamplingFactor;
            int endSample = juce::jmin(startSample + oversamplingFactor, inputSamples);
            
            for (int j = startSample; j < endSample; ++j)
            {
                sum += inputData[j];
            }
            
            outputData[i] = sum / oversamplingFactor;
        }
    }
}
