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
    routingMatrix.prepareToPlay(sampleRate, blockSize);

    // Initialize temporary buffers
    tempBuffer.setSize(2, blockSize);
    mixerBuffer.setSize(2, blockSize);
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
    routingMatrix.releaseResources();
}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Clear output buffer
    buffer.clear();

    // Process each sound source
    oscillator1.processBlock(tempBuffer, adsrOsc1);
    oscillator2.processBlock(tempBuffer, adsrOsc2);
    noiseRumble.processBlock(tempBuffer, adsrNoise);
    sampler.processBlock(tempBuffer, adsrSampler);
    subGenerator.processBlock(tempBuffer, adsrSub);

    // Mix all sources (simplified for now - will be replaced by proper mixer)
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Sum all sources (this is a placeholder)
            float mixedSample = 0.0f;
            // In a real implementation, we would mix from individual buffers
            buffer.addSample(channel, sample, mixedSample * 0.2f); // Scale down for safety
        }
    }

    // Apply processing chain: Mixer -> FilterBank -> Distortion -> Effects
    // For now, just pass through (will be implemented in later phases)
    filterBank.processBlock(buffer);
    distortionUnit.processBlock(buffer);
    compressor.processBlock(buffer);
    parametricEQ.processBlock(buffer);
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

    auto routingState = state.getOrCreateChildWithName("RoutingMatrix", nullptr);
    routingMatrix.saveState(routingState);
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

    if (auto routingState = state.getChildWithName("RoutingMatrix"))
        routingMatrix.loadState(*routingState);
}
