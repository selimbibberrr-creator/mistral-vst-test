#include "Oscillator.h"

Oscillator::Oscillator()
{
}

Oscillator::~Oscillator()
{
}

void Oscillator::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    phase = 0.0f;
    updatePhaseIncrement();
}

void Oscillator::releaseResources()
{
}

void Oscillator::noteOn(int noteNumber, float velocity)
{
    isNoteOn = true;
    currentNote = noteNumber;
    currentVelocity = velocity;
    phase = 0.0f; // Reset phase on new note
    
    // Convert MIDI note to frequency
    frequency = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
    updatePhaseIncrement();
}

void Oscillator::noteOff()
{
    isNoteOn = false;
}

void Oscillator::setWaveform(WaveformType type)
{
    waveformType = type;
}

void Oscillator::setFrequency(float freq)
{
    frequency = freq;
    updatePhaseIncrement();
}

void Oscillator::setPitchBend(float bend)
{
    pitchBend = bend;
    // Apply pitch bend (1 semitone per 1.0 bend)
    frequency = juce::MidiMessage::getMidiNoteInHertz(currentNote) * std::pow(2.0f, bend);
    updatePhaseIncrement();
}

void Oscillator::setLevel(float newLevel)
{
    level = juce::jlimit(0.0f, 1.0f, newLevel);
}

void Oscillator::setPan(float newPan)
{
    pan = juce::jlimit(0.0f, 1.0f, newPan);
}

void Oscillator::updatePhaseIncrement()
{
    phaseIncrement = frequency / static_cast<float>(sampleRate);
}

void Oscillator::saveState(juce::ValueTree& state)
{
    state.setProperty("waveform", static_cast<int>(waveformType), nullptr);
    state.setProperty("frequency", frequency, nullptr);
    state.setProperty("level", level, nullptr);
    state.setProperty("pan", pan, nullptr);
}

void Oscillator::loadState(const juce::ValueTree& state)
{
    if (auto waveform = state.getProperty("waveform"))
        waveformType = static_cast<WaveformType>(static_cast<int>(*waveform));
    
    if (auto freq = state.getProperty("frequency"))
        frequency = static_cast<float>(*freq);
    
    if (auto lvl = state.getProperty("level"))
        level = static_cast<float>(*lvl);
    
    if (auto p = state.getProperty("pan"))
        pan = static_cast<float>(*p);
    
    updatePhaseIncrement();
}
