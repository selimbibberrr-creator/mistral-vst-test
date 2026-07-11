#include "Sampler.h"
#include <JuceHeader.h>

Sampler::Sampler()
{
    waveformType = SINE; // Default, but will be overridden when sample is loaded
}

Sampler::~Sampler()
{
    unloadSample();
}

void Sampler::processBlock(juce::AudioBuffer<float>& buffer, ADSR& envelope)
{
    if (!isNoteOn || !hasSampleLoaded()) return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // If this is a new note, reset playback position
    if (!isPlaying)
    {
        isPlaying = true;
        playbackPosition = startPoint * sampleLength;
        updatePlaybackIncrement();
    }

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get sample from buffer
        float sampleValue = getSampleAtPosition(playbackPosition);
        
        // Apply envelope
        float envValue = envelope.getNextSample();
        sampleValue *= envValue * currentVelocity;

        // Apply level and pan
        float leftSample = sampleValue * level * (1.0f - pan);
        float rightSample = sampleValue * level * pan;

        // Add to buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.addSample(channel, sample, (channel == 0) ? leftSample : rightSample);
        }

        // Update playback position
        playbackPosition += playbackIncrement;
        
        // Check if we've reached the end
        if ((!reverse && playbackPosition >= endPoint * sampleLength) ||
            (reverse && playbackPosition <= startPoint * sampleLength))
        {
            isPlaying = false;
            break;
        }
    }
}

bool Sampler::loadSample(const juce::String& filePath)
{
    unloadSample();
    
    juce::File file(filePath);
    if (!file.existsAsFile()) return false;
    
    // Try to load as WAV file
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatReader> reader(wavFormat.createReaderFor(file, false));
    
    if (reader == nullptr) return false;
    
    sampleName = file.getFileNameWithoutExtension();
    sampleLength = static_cast<int>(reader->lengthInSamples);
    sampleChannels = reader->numChannels;
    sampleRate = static_cast<int>(reader->sampleRate);
    
    // Read sample data
    sampleData.resize(sampleLength * sampleChannels);
    reader->read(&sampleData[0], sampleLength, 0, sampleChannels, true);
    
    // Normalize sample data to mono if needed
    if (sampleChannels > 1)
    {
        std::vector<float> monoData(sampleLength);
        for (int i = 0; i < sampleLength; ++i)
        {
            float sum = 0.0f;
            for (int ch = 0; ch < sampleChannels; ++ch)
                sum += sampleData[i * sampleChannels + ch];
            monoData[i] = sum / sampleChannels;
        }
        sampleData = monoData;
        sampleChannels = 1;
    }
    
    updatePlaybackIncrement();
    return true;
}

bool Sampler::loadSample(const void* data, size_t dataSize)
{
    unloadSample();
    
    // For now, just copy the data (simplified)
    // In a real implementation, we would parse the audio format
    sampleData.resize(dataSize / sizeof(float));
    std::memcpy(sampleData.data(), data, dataSize);
    sampleLength = static_cast<int>(sampleData.size());
    sampleChannels = 1;
    sampleName = "Custom Sample";
    
    updatePlaybackIncrement();
    return true;
}

void Sampler::unloadSample()
{
    sampleData.clear();
    sampleLength = 0;
    sampleChannels = 1;
    sampleName = "";
    isPlaying = false;
}

void Sampler::setPitch(float semitones)
{
    pitchSemitones = semitones;
    updatePlaybackIncrement();
}

void Sampler::setStartPoint(float start)
{
    startPoint = juce::jlimit(0.0f, 1.0f, start);
}

void Sampler::setEndPoint(float end)
{
    endPoint = juce::jlimit(0.0f, 1.0f, end);
}

void Sampler::setReverse(bool shouldReverse)
{
    reverse = shouldReverse;
    updatePlaybackIncrement();
}

void Sampler::setTimeStretch(float factor)
{
    timeStretch = juce::jlimit(0.5f, 2.0f, factor);
    updatePlaybackIncrement();
}

void Sampler::updatePlaybackIncrement()
{
    // Calculate base increment
    float baseIncrement = 1.0f;
    
    // Apply pitch shift
    float pitchFactor = std::pow(2.0f, pitchSemitones / 12.0f);
    baseIncrement *= pitchFactor;
    
    // Apply time stretch
    baseIncrement /= timeStretch;
    
    // Apply direction
    if (reverse)
        playbackIncrement = -baseIncrement;
    else
        playbackIncrement = baseIncrement;
}

float Sampler::getSampleAtPosition(float position)
{
    if (sampleData.empty()) return 0.0f;
    
    // Clamp position
    position = juce::jlimit(0.0f, static_cast<float>(sampleLength - 1), position);
    
    // Get integer position
    int intPosition = static_cast<int>(position);
    
    // Linear interpolation
    if (intPosition < sampleLength - 1)
    {
        float frac = position - intPosition;
        return juce::jmap(frac, 0.0f, 1.0f, 
                         sampleData[intPosition], 
                         sampleData[intPosition + 1]);
    }
    
    return sampleData[intPosition];
}

void Sampler::noteOn(int noteNumber, float velocity)
{
    Oscillator::noteOn(noteNumber, velocity);
    isPlaying = false; // Reset for new note
}

void Sampler::noteOff()
{
    Oscillator::noteOff();
    isPlaying = false;
}

void Sampler::saveState(juce::ValueTree& state)
{
    Oscillator::saveState(state);
    state.setProperty("pitch", pitchSemitones, nullptr);
    state.setProperty("startPoint", startPoint, nullptr);
    state.setProperty("endPoint", endPoint, nullptr);
    state.setProperty("reverse", reverse, nullptr);
    state.setProperty("timeStretch", timeStretch, nullptr);
    state.setProperty("sampleName", sampleName, nullptr);
}

void Sampler::loadState(const juce::ValueTree& state)
{
    Oscillator::loadState(state);
    
    if (auto pitch = state.getProperty("pitch"))
        pitchSemitones = static_cast<float>(*pitch);
    
    if (auto start = state.getProperty("startPoint"))
        startPoint = static_cast<float>(*start);
    
    if (auto end = state.getProperty("endPoint"))
        endPoint = static_cast<float>(*end);
    
    if (auto rev = state.getProperty("reverse"))
        reverse = static_cast<bool>(*rev);
    
    if (auto ts = state.getProperty("timeStretch"))
        timeStretch = static_cast<float>(*ts);
    
    if (auto name = state.getProperty("sampleName"))
        sampleName = static_cast<juce::String>(*name);
    
    updatePlaybackIncrement();
}
