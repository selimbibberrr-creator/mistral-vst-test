#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    // Initialize preset manager with this engine
    presetManager.initialize(this);
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;

    // Prepare all modules
    oscillator1.prepareToPlay(sampleRate, blockSize);
    oscillator2.prepareToPlay(sampleRate, blockSize);
    noiseRumble.prepareToPlay(sampleRate, blockSize);
    sampler.prepareToPlay(sampleRate, blockSize);
    subGenerator.prepareToPlay(sampleRate, blockSize);

    adsrOsc1.prepareToPlay(sampleRate, blockSize);
    adsrOsc2.prepareToPlay(sampleRate, blockSize);
    adsrNoise.prepareToPlay(sampleRate, blockSize);
    adsrSampler.prepareToPlay(sampleRate, blockSize);
    adsrSub.prepareToPlay(sampleRate, blockSize);

    mixer.prepareToPlay(sampleRate, blockSize);
    filterBank.prepareToPlay(sampleRate, blockSize);
    distortionUnit.prepareToPlay(sampleRate, blockSize);
    compressor.prepareToPlay(sampleRate, blockSize);
    parametricEQ.prepareToPlay(sampleRate, blockSize);
    reverb.prepareToPlay(sampleRate, blockSize);
    delay.prepareToPlay(sampleRate, blockSize);
    stereoImager.prepareToPlay(sampleRate, blockSize);
    routingMatrix.prepareToPlay(sampleRate, blockSize);

    // Initialize temporary buffers
    mainBuffer.setSize(2, blockSize);
    
    for (int i = 0; i < 5; ++i)
    {
        sourceBuffers[i].setSize(2, blockSize);
    }
}

void AudioEngine::releaseResources()
{
    oscillator1.releaseResources();
    oscillator2.releaseResources();
    noiseRumble.releaseResources();
    sampler.releaseResources();
    subGenerator.releaseResources();

    mixer.releaseResources();
    filterBank.releaseResources();
    distortionUnit.releaseResources();
    compressor.releaseResources();
    parametricEQ.releaseResources();
    reverb.releaseResources();
    delay.releaseResources();
    stereoImager.releaseResources();
    routingMatrix.releaseResources();
}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Clear output buffer
    buffer.clear();

    // Process each sound source into individual buffers
    oscillator1.processBlock(sourceBuffers[0], adsrOsc1);
    oscillator2.processBlock(sourceBuffers[1], adsrOsc2);
    noiseRumble.processBlock(sourceBuffers[2], adsrNoise);
    sampler.processBlock(sourceBuffers[3], adsrSampler);
    subGenerator.processBlock(sourceBuffers[4], adsrSub);

    // Mix all sources
    mixer.processBlock(mainBuffer, 
                       sourceBuffers[0], 
                       sourceBuffers[1], 
                       sourceBuffers[2], 
                       sourceBuffers[3], 
                       sourceBuffers[4]);

    // Apply processing chain based on routing matrix
    // For now, use the default signal flow:
    // Mixer -> FilterBank -> Distortion -> Compressor -> EQ -> Reverb -> Delay -> StereoImager
    
    // Apply filter bank
    filterBank.processBlock(mainBuffer);
    
    // Apply distortion
    distortionUnit.processBlock(mainBuffer);
    
    // Apply compressor
    compressor.processBlock(mainBuffer);
    
    // Apply EQ
    parametricEQ.processBlock(mainBuffer);
    
    // Apply reverb
    reverb.processBlock(mainBuffer);
    
    // Apply delay
    delay.processBlock(mainBuffer);
    
    // Apply stereo imager
    stereoImager.processBlock(mainBuffer);
    
    // Copy to output
    buffer.makeCopyOf(mainBuffer);
}

void AudioEngine::processMidiMessage(const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        // Trigger all ADSR envelopes
        adsrOsc1.noteOn();
        adsrOsc2.noteOn();
        adsrNoise.noteOn();
        adsrSampler.noteOn();
        adsrSub.noteOn();

        // Trigger oscillators
        oscillator1.noteOn(message.getNoteNumber(), message.getVelocity());
        oscillator2.noteOn(message.getNoteNumber(), message.getVelocity());
        subGenerator.noteOn(message.getNoteNumber(), message.getVelocity());
        sampler.noteOn(message.getNoteNumber(), message.getVelocity());
        noiseRumble.noteOn(message.getNoteNumber(), message.getVelocity());
    }
    else if (message.isNoteOff())
    {
        // Release all ADSR envelopes
        adsrOsc1.noteOff();
        adsrOsc2.noteOff();
        adsrNoise.noteOff();
        adsrSampler.noteOff();
        adsrSub.noteOff();

        // Release oscillators
        oscillator1.noteOff();
        oscillator2.noteOff();
        subGenerator.noteOff();
        sampler.noteOff();
        noiseRumble.noteOff();
    }
    else if (message.isPitchWheel())
    {
        // Handle pitch bend
        float pitchBend = message.getPitchWheelValue() / 8192.0f; // -1 to +1
        oscillator1.setPitchBend(pitchBend);
        oscillator2.setPitchBend(pitchBend);
        subGenerator.setPitchBend(pitchBend);
        sampler.setPitchBend(pitchBend);
    }
}

void AudioEngine::saveState(juce::ValueTree& state)
{
    // Save all module states
    auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
    oscillator1.saveState(osc1State);

    auto osc2State = state.getOrCreateChildWithName("Oscillator2", nullptr);
    oscillator2.saveState(osc2State);

    auto noiseState = state.getOrCreateChildWithName("NoiseRumble", nullptr);
    noiseRumble.saveState(noiseState);

    auto samplerState = state.getOrCreateChildWithName("Sampler", nullptr);
    sampler.saveState(samplerState);

    auto subState = state.getOrCreateChildWithName("SubGenerator", nullptr);
    subGenerator.saveState(subState);

    auto mixerState = state.getOrCreateChildWithName("Mixer", nullptr);
    mixer.saveState(mixerState);

    auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
    filterBank.saveState(filterState);

    auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
    distortionUnit.saveState(distortionState);

    auto compressorState = state.getOrCreateChildWithName("Compressor", nullptr);
    compressor.saveState(compressorState);

    auto eqState = state.getOrCreateChildWithName("ParametricEQ", nullptr);
    parametricEQ.saveState(eqState);

    auto reverbState = state.getOrCreateChildWithName("Reverb", nullptr);
    reverb.saveState(reverbState);

    auto delayState = state.getOrCreateChildWithName("Delay", nullptr);
    delay.saveState(delayState);

    auto stereoState = state.getOrCreateChildWithName("StereoImager", nullptr);
    stereoImager.saveState(stereoState);

    auto routingState = state.getOrCreateChildWithName("RoutingMatrix", nullptr);
    routingMatrix.saveState(routingState);

    // Save ADSR states
    auto adsrOsc1State = state.getOrCreateChildWithName("ADSR_Osc1", nullptr);
    adsrOsc1.saveState(adsrOsc1State);

    auto adsrOsc2State = state.getOrCreateChildWithName("ADSR_Osc2", nullptr);
    adsrOsc2.saveState(adsrOsc2State);

    auto adsrNoiseState = state.getOrCreateChildWithName("ADSR_Noise", nullptr);
    adsrNoise.saveState(adsrNoiseState);

    auto adsrSamplerState = state.getOrCreateChildWithName("ADSR_Sampler", nullptr);
    adsrSampler.saveState(adsrSamplerState);

    auto adsrSubState = state.getOrCreateChildWithName("ADSR_Sub", nullptr);
    adsrSub.saveState(adsrSubState);

    // Save preset manager state
    auto presetState = state.getOrCreateChildWithName("PresetManager", nullptr);
    presetManager.saveState(presetState);
}

void AudioEngine::loadState(const juce::ValueTree& state)
{
    // Load all module states
    if (auto osc1State = state.getChildWithName("Oscillator1"))
        oscillator1.loadState(*osc1State);

    if (auto osc2State = state.getChildWithName("Oscillator2"))
        oscillator2.loadState(*osc2State);

    if (auto noiseState = state.getChildWithName("NoiseRumble"))
        noiseRumble.loadState(*noiseState);

    if (auto samplerState = state.getChildWithName("Sampler"))
        sampler.loadState(*samplerState);

    if (auto subState = state.getChildWithName("SubGenerator"))
        subGenerator.loadState(*subState);

    if (auto mixerState = state.getChildWithName("Mixer"))
        mixer.loadState(*mixerState);

    if (auto filterState = state.getChildWithName("FilterBank"))
        filterBank.loadState(*filterState);

    if (auto distortionState = state.getChildWithName("DistortionUnit"))
        distortionUnit.loadState(*distortionState);

    if (auto compressorState = state.getChildWithName("Compressor"))
        compressor.loadState(*compressorState);

    if (auto eqState = state.getChildWithName("ParametricEQ"))
        parametricEQ.loadState(*eqState);

    if (auto reverbState = state.getChildWithName("Reverb"))
        reverb.loadState(*reverbState);

    if (auto delayState = state.getChildWithName("Delay"))
        delay.loadState(*delayState);

    if (auto stereoState = state.getChildWithName("StereoImager"))
        stereoImager.loadState(*stereoState);

    if (auto routingState = state.getChildWithName("RoutingMatrix"))
        routingMatrix.loadState(*routingState);

    // Load ADSR states
    if (auto adsrOsc1State = state.getChildWithName("ADSR_Osc1"))
        adsrOsc1.loadState(*adsrOsc1State);

    if (auto adsrOsc2State = state.getChildWithName("ADSR_Osc2"))
        adsrOsc2.loadState(*adsrOsc2State);

    if (auto adsrNoiseState = state.getChildWithName("ADSR_Noise"))
        adsrNoise.loadState(*adsrNoiseState);

    if (auto adsrSamplerState = state.getChildWithName("ADSR_Sampler"))
        adsrSampler.loadState(*adsrSamplerState);

    if (auto adsrSubState = state.getChildWithName("ADSR_Sub"))
        adsrSub.loadState(*adsrSubState);

    // Load preset manager state
    if (auto presetState = state.getChildWithName("PresetManager"))
        presetManager.loadState(*presetState);
}
