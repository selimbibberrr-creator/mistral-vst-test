#include "DistortionUnit.h"
#include "SoftClip.h"
#include "HardClip.h"
#include "Chebyshev.h"
#include "Bitcrush.h"
#include "Tape.h"
#include "Foldback.h"

DistortionUnit::DistortionUnit()
{
    // Initialize all algorithms
    algorithms[SOFT_CLIP] = std::make_unique<SoftClip>();
    algorithms[HARD_CLIP] = std::make_unique<HardClip>();
    algorithms[CHEBYSHEV] = std::make_unique<Chebyshev>();
    algorithms[BITCRUSH] = std::make_unique<Bitcrush>();
    algorithms[TAPE] = std::make_unique<Tape>();
    algorithms[FOLDBACK] = std::make_unique<Foldback>();
}

DistortionUnit::~DistortionUnit()
{
}

void DistortionUnit::prepareToPlay(double newSampleRate, int newBlockSize)
{
    // Prepare all algorithms
    for (auto& algo : algorithms)
    {
        algo->prepareToPlay(newSampleRate, newBlockSize);
    }
    
    // Initialize buffers
    oversampledBuffer.setSize(2, newBlockSize * 4); // Max 4x oversampling
    downsampledBuffer.setSize(2, newBlockSize);
    lowBandBuffer.setSize(2, newBlockSize);
    midBandBuffer.setSize(2, newBlockSize);
    highBandBuffer.setSize(2, newBlockSize);
}

void DistortionUnit::releaseResources()
{
    for (auto& algo : algorithms)
    {
        algo->releaseResources();
    }
}

void DistortionUnit::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (multibandEnabled)
    {
        processMultiband(buffer);
    }
    else
    {
        processSingleBand(buffer);
    }
}

void DistortionUnit::processSingleBand(juce::AudioBuffer<float>& buffer)
{
    // Apply output gain before distortion
    if (output != 0.0f)
    {
        float gain = juce::Decibels::decibelsToGain(output);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= gain;
            }
        }
    }
    
    // Apply distortion
    algorithms[currentAlgorithm]->processBlock(buffer, drive, mix);
}

void DistortionUnit::processMultiband(juce::AudioBuffer<float>& buffer)
{
    // Split into frequency bands
    applyCrossover(buffer, lowBandBuffer, midBandBuffer, highBandBuffer);
    
    // Process each band with its own algorithm
    algorithms[lowAlgorithm]->processBlock(lowBandBuffer, lowDrive, mix);
    algorithms[midAlgorithm]->processBlock(midBandBuffer, midDrive, mix);
    algorithms[highAlgorithm]->processBlock(highBandBuffer, highDrive, mix);
    
    // Combine bands
    combineBands(buffer, lowBandBuffer, midBandBuffer, highBandBuffer);
    
    // Apply output gain
    if (output != 0.0f)
    {
        float gain = juce::Decibels::decibelsToGain(output);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= gain;
            }
        }
    }
}

void DistortionUnit::applyCrossover(juce::AudioBuffer<float>& buffer,
                                   juce::AudioBuffer<float>& lowBand,
                                   juce::AudioBuffer<float>& midBand,
                                   juce::AudioBuffer<float>& highBand)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Resize buffers if needed
    lowBand.setSize(numChannels, numSamples, false, false, true);
    midBand.setSize(numChannels, numSamples, false, false, true);
    highBand.setSize(numChannels, numSamples, false, false, true);
    
    // Simple crossover using first-order filters
    // In a production implementation, this would use Linkwitz-Riley filters
    
    float lowOmega = 2.0f * juce::MathConstants<float>::pi * lowMidCrossover / static_cast<float>(44100.0);
    float midOmega = 2.0f * juce::MathConstants<float>::pi * midHighCrossover / static_cast<float>(44100.0);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* inData = buffer.getReadPointer(channel);
        auto* lowData = lowBand.getWritePointer(channel);
        auto* midData = midBand.getWritePointer(channel);
        auto* highData = highBand.getWritePointer(channel);
        
        // Simple state for filters
        float lowZ = 0.0f;
        float midZ1 = 0.0f;
        float midZ2 = 0.0f;
        float highZ = 0.0f;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = inData[sample];
            
            // Lowpass for low band
            lowZ = lowZ + lowOmega * (input - lowZ);
            lowData[sample] = lowZ;
            
            // Bandpass for mid band (difference between two lowpasses)
            midZ1 = midZ1 + lowOmega * (input - midZ1);
            midZ2 = midZ2 + midOmega * (input - midZ2);
            midData[sample] = midZ2 - midZ1;
            
            // Highpass for high band
            highZ = highZ + midOmega * (input - highZ);
            highData[sample] = input - highZ;
        }
    }
}

void DistortionUnit::combineBands(juce::AudioBuffer<float>& buffer,
                                  const juce::AudioBuffer<float>& lowBand,
                                  const juce::AudioBuffer<float>& midBand,
                                  const juce::AudioBuffer<float>& highBand)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* outData = buffer.getWritePointer(channel);
        auto* lowData = lowBand.getReadPointer(channel);
        auto* midData = midBand.getReadPointer(channel);
        auto* highData = highBand.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            outData[sample] = lowData[sample] + midData[sample] + highData[sample];
        }
    }
}

void DistortionUnit::setAlgorithm(AlgorithmType algorithm)
{
    currentAlgorithm = algorithm;
}

void DistortionUnit::setDrive(float newDrive)
{
    drive = juce::jlimit(0.0f, 1.0f, newDrive);
}

void DistortionUnit::setOutput(float newOutput)
{
    output = juce::jlimit(-24.0f, 12.0f, newOutput);
}

void DistortionUnit::setMix(float newMix)
{
    mix = juce::jlimit(0.0f, 1.0f, newMix);
}

void DistortionUnit::setOversampling(int factor)
{
    oversampling = juce::jlimit(1, 4, factor);
}

void DistortionUnit::setMultibandEnabled(bool enabled)
{
    multibandEnabled = enabled;
}

void DistortionUnit::setLowMidCrossover(float frequency)
{
    lowMidCrossover = juce::jlimit(20.0f, 2000.0f, frequency);
}

void DistortionUnit::setMidHighCrossover(float frequency)
{
    midHighCrossover = juce::jlimit(200.0f, 20000.0f, frequency);
}

void DistortionUnit::setLowAlgorithm(AlgorithmType algorithm)
{
    lowAlgorithm = algorithm;
}

void DistortionUnit::setMidAlgorithm(AlgorithmType algorithm)
{
    midAlgorithm = algorithm;
}

void DistortionUnit::setHighAlgorithm(AlgorithmType algorithm)
{
    highAlgorithm = algorithm;
}

void DistortionUnit::setLowDrive(float drive)
{
    lowDrive = juce::jlimit(0.0f, 1.0f, drive);
}

void DistortionUnit::setMidDrive(float drive)
{
    midDrive = juce::jlimit(0.0f, 1.0f, drive);
}

void DistortionUnit::setHighDrive(float drive)
{
    highDrive = juce::jlimit(0.0f, 1.0f, drive);
}

void DistortionUnit::saveState(juce::ValueTree& state)
{
    state.setProperty("algorithm", static_cast<int>(currentAlgorithm), nullptr);
    state.setProperty("drive", drive, nullptr);
    state.setProperty("output", output, nullptr);
    state.setProperty("mix", mix, nullptr);
    state.setProperty("oversampling", oversampling, nullptr);
    
    state.setProperty("multibandEnabled", multibandEnabled, nullptr);
    state.setProperty("lowMidCrossover", lowMidCrossover, nullptr);
    state.setProperty("midHighCrossover", midHighCrossover, nullptr);
    
    state.setProperty("lowAlgorithm", static_cast<int>(lowAlgorithm), nullptr);
    state.setProperty("midAlgorithm", static_cast<int>(midAlgorithm), nullptr);
    state.setProperty("highAlgorithm", static_cast<int>(highAlgorithm), nullptr);
    
    state.setProperty("lowDrive", lowDrive, nullptr);
    state.setProperty("midDrive", midDrive, nullptr);
    state.setProperty("highDrive", highDrive, nullptr);
}

void DistortionUnit::loadState(const juce::ValueTree& state)
{
    if (auto alg = state.getProperty("algorithm"))
        currentAlgorithm = static_cast<AlgorithmType>(static_cast<int>(*alg));
    
    if (auto d = state.getProperty("drive"))
        drive = static_cast<float>(*d);
    
    if (auto o = state.getProperty("output"))
        output = static_cast<float>(*o);
    
    if (auto m = state.getProperty("mix"))
        mix = static_cast<float>(*m);
    
    if (auto os = state.getProperty("oversampling"))
        oversampling = static_cast<int>(*os);
    
    if (auto mb = state.getProperty("multibandEnabled"))
        multibandEnabled = static_cast<bool>(*mb);
    
    if (auto lmc = state.getProperty("lowMidCrossover"))
        lowMidCrossover = static_cast<float>(*lmc);
    
    if (auto mhc = state.getProperty("midHighCrossover"))
        midHighCrossover = static_cast<float>(*mhc);
    
    if (auto la = state.getProperty("lowAlgorithm"))
        lowAlgorithm = static_cast<AlgorithmType>(static_cast<int>(*la));
    
    if (auto ma = state.getProperty("midAlgorithm"))
        midAlgorithm = static_cast<AlgorithmType>(static_cast<int>(*ma));
    
    if (auto ha = state.getProperty("highAlgorithm"))
        highAlgorithm = static_cast<AlgorithmType>(static_cast<int>(*ha));
    
    if (auto ld = state.getProperty("lowDrive"))
        lowDrive = static_cast<float>(*ld);
    
    if (auto md = state.getProperty("midDrive"))
        midDrive = static_cast<float>(*md);
    
    if (auto hd = state.getProperty("highDrive"))
        highDrive = static_cast<float>(*hd);
}
