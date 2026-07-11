#pragma once

#include <JuceHeader.h>
#include "Oscillators/Oscillator1.h"
#include "Oscillators/Oscillator2.h"
#include "Oscillators/NoiseRumble.h"
#include "Oscillators/Sampler.h"
#include "Oscillators/SubGenerator.h"
#include "Envelopes/ADSR.h"
#include "Mixer/Mixer.h"
#include "Filters/FilterBank.h"
#include "Distortion/DistortionUnit.h"
#include "Effects/Compressor.h"
#include "Effects/ParametricEQ.h"
#include "Effects/Reverb.h"
#include "Effects/Delay.h"
#include "Effects/StereoImager.h"
#include "Routing/RoutingMatrix.h"
#include "Presets/PresetManager.h"

class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);
    void processMidiMessage(const juce::MidiMessage& message);

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

    // Access to modules
    Oscillator1& getOscillator1() { return oscillator1; }
    Oscillator2& getOscillator2() { return oscillator2; }
    NoiseRumble& getNoiseRumble() { return noiseRumble; }
    Sampler& getSampler() { return sampler; }
    SubGenerator& getSubGenerator() { return subGenerator; }
    Mixer& getMixer() { return mixer; }
    FilterBank& getFilterBank() { return filterBank; }
    DistortionUnit& getDistortionUnit() { return distortionUnit; }
    Compressor& getCompressor() { return compressor; }
    ParametricEQ& getParametricEQ() { return parametricEQ; }
    Reverb& getReverb() { return reverb; }
    Delay& getDelay() { return delay; }
    StereoImager& getStereoImager() { return stereoImager; }
    RoutingMatrix& getRoutingMatrix() { return routingMatrix; }
    PresetManager& getPresetManager() { return presetManager; }

    // ADSR envelopes access
    ADSR& getADSR_Osc1() { return adsrOsc1; }
    ADSR& getADSR_Osc2() { return adsrOsc2; }
    ADSR& getADSR_Noise() { return adsrNoise; }
    ADSR& getADSR_Sampler() { return adsrSampler; }
    ADSR& getADSR_Sub() { return adsrSub; }

private:
    // Sample rate and block size
    double sampleRate = 44100.0;
    int blockSize = 512;

    // Sound sources
    Oscillator1 oscillator1;
    Oscillator2 oscillator2;
    NoiseRumble noiseRumble;
    Sampler sampler;
    SubGenerator subGenerator;

    // ADSR envelopes for each source
    ADSR adsrOsc1;
    ADSR adsrOsc2;
    ADSR adsrNoise;
    ADSR adsrSampler;
    ADSR adsrSub;

    // Mixer
    Mixer mixer;

    // Processing modules
    FilterBank filterBank;
    DistortionUnit distortionUnit;
    Compressor compressor;
    ParametricEQ parametricEQ;
    Reverb reverb;
    Delay delay;
    StereoImager stereoImager;
    RoutingMatrix routingMatrix;

    // Preset manager
    PresetManager presetManager;

    // Temporary buffers for processing
    juce::AudioBuffer<float> sourceBuffers[5]; // Individual source buffers
    juce::AudioBuffer<float> mainBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
