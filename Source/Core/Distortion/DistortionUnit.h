#pragma once

#include <JuceHeader.h>
#include "DistortionAlgorithm.h"

// Main distortion unit with 6 algorithms and multiband support
class DistortionUnit
{
public:
    enum AlgorithmType
    {
        SOFT_CLIP,
        HARD_CLIP,
        CHEBYSHEV,
        BITCRUSH,
        TAPE,
        FOLDBACK
    };

    DistortionUnit();
    ~DistortionUnit();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Algorithm selection
    void setAlgorithm(AlgorithmType algorithm);
    AlgorithmType getAlgorithm() const { return currentAlgorithm; }

    // Parameters
    void setDrive(float drive); // 0.0 to 1.0
    void setOutput(float output); // -24dB to +12dB
    void setMix(float mix); // Dry/Wet mix (0.0 to 1.0)
    void setOversampling(int factor); // 1x, 2x, 4x

    // Getters
    float getDrive() const { return drive; }
    float getOutput() const { return output; }
    float getMix() const { return mix; }
    int getOversampling() const { return oversampling; }

    // Multiband mode
    void setMultibandEnabled(bool enabled);
    bool isMultibandEnabled() const { return multibandEnabled; }

    // Crossover frequencies
    void setLowMidCrossover(float frequency); // 20-2000Hz
    void setMidHighCrossover(float frequency); // 200-20000Hz

    // Per-band algorithm selection
    void setLowAlgorithm(AlgorithmType algorithm);
    void setMidAlgorithm(AlgorithmType algorithm);
    void setHighAlgorithm(AlgorithmType algorithm);

    // Per-band parameters
    void setLowDrive(float drive);
    void setMidDrive(float drive);
    void setHighDrive(float drive);

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    // Current settings
    AlgorithmType currentAlgorithm = SOFT_CLIP;
    float drive = 0.5f;
    float output = 0.0f; // 0dB
    float mix = 0.5f; // 50% wet
    int oversampling = 1; // 1x

    // Multiband settings
    bool multibandEnabled = false;
    float lowMidCrossover = 200.0f; // 200Hz
    float midHighCrossover = 2000.0f; // 2kHz

    // Per-band settings
    AlgorithmType lowAlgorithm = SOFT_CLIP;
    AlgorithmType midAlgorithm = HARD_CLIP;
    AlgorithmType highAlgorithm = CHEBYSHEV;
    
    float lowDrive = 0.5f;
    float midDrive = 0.5f;
    float highDrive = 0.5f;

    // Algorithm instances
    std::unique_ptr<DistortionAlgorithm> algorithms[6];
    
    // Multiband processing
    void processMultiband(juce::AudioBuffer<float>& buffer);
    void processSingleBand(juce::AudioBuffer<float>& buffer);

    // Oversampling buffers
    juce::AudioBuffer<float> oversampledBuffer;
    juce::AudioBuffer<float> downsampledBuffer;

    // Crossover filters
    void applyCrossover(juce::AudioBuffer<float>& buffer, 
                       juce::AudioBuffer<float>& lowBand,
                       juce::AudioBuffer<float>& midBand,
                       juce::AudioBuffer<float>& highBand);
    void combineBands(juce::AudioBuffer<float>& buffer,
                     const juce::AudioBuffer<float>& lowBand,
                     const juce::AudioBuffer<float>& midBand,
                     const juce::AudioBuffer<float>& highBand);

    // Temporary buffers for multiband processing
    juce::AudioBuffer<float> lowBandBuffer;
    juce::AudioBuffer<float> midBandBuffer;
    juce::AudioBuffer<float> highBandBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionUnit)
};
