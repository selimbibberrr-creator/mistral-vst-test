#pragma once

#include <JuceHeader.h>

// Modular routing matrix for flexible signal routing
// 16x16 grid for connecting modules
class RoutingMatrix
{
public:
    // Module indices
    enum ModuleIndex
    {
        OSC1,
        OSC2,
        NOISE,
        SAMPLER,
        SUB,
        MIXER,
        FILTER_A,
        FILTER_B,
        DISTORTION,
        COMPRESSOR,
        EQ,
        REVERB,
        DELAY,
        STEREO_IMAGER,
        MASTER_OUT,
        NUM_MODULES
    };

    RoutingMatrix();
    ~RoutingMatrix();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Connection management
    void connect(ModuleIndex source, ModuleIndex destination, float gain = 1.0f);
    void disconnect(ModuleIndex source, ModuleIndex destination);
    void disconnectAll(ModuleIndex module);
    void setGain(ModuleIndex source, ModuleIndex destination, float gain);
    float getGain(ModuleIndex source, ModuleIndex destination) const;
    bool isConnected(ModuleIndex source, ModuleIndex destination) const;

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    // Connection matrix: [source][destination] = gain
    float connectionMatrix[NUM_MODULES][NUM_MODULES];

    // Temporary buffers for each module
    juce::AudioBuffer<float> moduleBuffers[NUM_MODULES];

    // Initialize default routing
    void initializeDefaultRouting();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoutingMatrix)
};
