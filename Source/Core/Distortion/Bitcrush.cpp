#include "Bitcrush.h"
#include <cmath>
#include <random>

Bitcrush::Bitcrush()
{
    updateSampleIncrement();
}

Bitcrush::~Bitcrush()
{
}

void Bitcrush::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Update bit depth based on drive
    int effectiveBitDepth = juce::jlimit(1, 16, static_cast<int>(juce::jmap(drive, 0.0f, 1.0f, 16.0f, 1.0f)));
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float original = channelData[sample];
            float distorted = processSample(original, drive);
            channelData[sample] = original * (1.0f - mix) + distorted * mix;
        }
    }
}

float Bitcrush::processSample(float sample, float drive)
{
    // Update bit depth based on drive
    int effectiveBitDepth = juce::jlimit(1, 16, static_cast<int>(juce::jmap(drive, 0.0f, 1.0f, 16.0f, 1.0f)));
    
    // Sample rate reduction
    sampleCounter += sampleIncrement;
    if (sampleCounter >= 1.0f)
    {
        sampleCounter -= 1.0f;
        lastSample = sample;
    }
    
    // Bit depth reduction
    float crushed = lastSample;
    
    if (effectiveBitDepth < 16)
    {
        // Calculate quantization step
        float step = 2.0f / (static_cast<float>(1 << effectiveBitDepth));
        
        // Quantize the sample
        float quantized = std::floor(crushed / step + 0.5f) * step;
        
        // Add dither (simple noise)
        static std::mt19937 gen(std::random_device{}());
        static std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
        float dither = dist(gen) * step * 0.5f;
        
        crushed = quantized + dither;
    }
    
    return crushed;
}

void Bitcrush::setBitDepth(int depth)
{
    bitDepth = juce::jlimit(1, 16, depth);
}

void Bitcrush::setSampleRate(float rate)
{
    crushSampleRate = juce::jlimit(1000.0f, 44100.0f, rate);
    updateSampleIncrement();
}

void Bitcrush::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: // Bit depth
            bitDepth = juce::jlimit(1, 16, static_cast<int>(value));
            break;
        case 1: // Sample rate
            crushSampleRate = juce::jlimit(1000.0f, 44100.0f, value);
            updateSampleIncrement();
            break;
    }
}

float Bitcrush::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return static_cast<float>(bitDepth);
        case 1: return crushSampleRate;
        default: return 0.0f;
    }
}

void Bitcrush::updateSampleIncrement()
{
    sampleIncrement = crushSampleRate / static_cast<float>(sampleRate);
}

void Bitcrush::saveState(juce::ValueTree& state)
{
    state.setProperty("bitDepth", bitDepth, nullptr);
    state.setProperty("sampleRate", crushSampleRate, nullptr);
}

void Bitcrush::loadState(const juce::ValueTree& state)
{
    if (auto bd = state.getProperty("bitDepth"))
        bitDepth = static_cast<int>(*bd);
    
    if (auto sr = state.getProperty("sampleRate"))
        crushSampleRate = static_cast<float>(*sr);
    
    updateSampleIncrement();
}
