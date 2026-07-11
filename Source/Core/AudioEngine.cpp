#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
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

    filterBank.prepareToPlay(sampleRate, blockSize);
    distortionUnit.prepareToPlay(sampleRate, blockSize);
    compressor.prepareToPlay(sampleRate, blockSize);
    parametricEQ.prepareToPlay(sampleRate, blockSize);
    reverb.prepareToPlay(sampleRate, blockSize);
    delay.prepareToPlay(sampleRate, blockSize);
    stereoImager.prepareToPlay(sampleRate, blockSize);
    routingMatrix.prepareToPlay(sampleRate, blockSize);

    // Initialize temporary buffers
    tempBuffer.setSize(2, blockSize);
    mixerBuffer.setSize(2, blockSize);
    
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

    // Mix all sources (simplified - will be replaced by proper mixer)
    mixerBuffer.clear();
    for (int i = 0; i < 5; ++i)
    {
        for (int channel = 0; channel < 2; ++channel)
        {
            auto* sourceData = sourceBuffers[i].getReadPointer(channel);
            auto* mixData = mixerBuffer.getWritePointer(channel);
            
            for (int sample = 0; sample < blockSize; ++sample)
            {
                mixData[sample] += sourceData[sample] * 0.2f; // Scale down for safety
            }
        }
    }

    // Apply processing chain: Mixer -> FilterBank -> Distortion -> Effects
    // For now, just pass through the mixer buffer
    
    // Apply filter bank
    filterBank.processBlock(mixerBuffer);
    
    // Apply distortion
    distortionUnit.processBlock(mixerBuffer);
    
    // Apply compressor
    compressor.processBlock(mixerBuffer);
    
    // Apply EQ
    parametricEQ.processBlock(mixerBuffer);
    
    // Apply reverb
    reverb.processBlock(mixerBuffer);
    
    // Apply delay
    delay.processBlock(mixerBuffer);
    
    // Apply stereo imager
    stereoImager.processBlock(mixerBuffer);
    
    // Copy to output
    buffer.makeCopyOf(mixerBuffer);
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
}
