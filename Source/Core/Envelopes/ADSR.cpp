#include "ADSR.h"

ADSR::ADSR()
{
}

ADSR::~ADSR()
{
}

void ADSR::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    updateIncrements();
}

void ADSR::releaseResources()
{
}

void ADSR::noteOn()
{
    if (state == EnvelopeState::IDLE)
    {
        state = EnvelopeState::ATTACK;
        currentValue = 0.0f;
        updateIncrements();
    }
}

void ADSR::noteOff()
{
    if (state != EnvelopeState::IDLE && state != EnvelopeState::RELEASE)
    {
        state = EnvelopeState::RELEASE;
        updateIncrements();
    }
}

void ADSR::reset()
{
    state = EnvelopeState::IDLE;
    currentValue = 0.0f;
}

void ADSR::setAttack(float timeMs)
{
    attackTime = juce::jmax(0.0f, timeMs);
    updateIncrements();
}

void ADSR::setDecay(float timeMs)
{
    decayTime = juce::jmax(0.0f, timeMs);
    updateIncrements();
}

void ADSR::setSustain(float level)
{
    sustainLevel = juce::jlimit(0.0f, 1.0f, level);
}

void ADSR::setRelease(float timeMs)
{
    releaseTime = juce::jmax(0.0f, timeMs);
    updateIncrements();
}

float ADSR::getNextSample()
{
    switch (state)
    {
        case EnvelopeState::IDLE:
            return 0.0f;

        case EnvelopeState::ATTACK:
        {
            currentValue += attackIncrement;
            if (currentValue >= 1.0f)
            {
                currentValue = 1.0f;
                state = EnvelopeState::DECAY;
                updateIncrements();
            }
            return currentValue;
        }

        case EnvelopeState::DECAY:
        {
            currentValue -= decayIncrement;
            if (currentValue <= sustainLevel)
            {
                currentValue = sustainLevel;
                state = EnvelopeState::SUSTAIN;
            }
            return currentValue;
        }

        case EnvelopeState::SUSTAIN:
            return sustainLevel;

        case EnvelopeState::RELEASE:
        {
            currentValue -= releaseIncrement;
            if (currentValue <= 0.0f)
            {
                currentValue = 0.0f;
                state = EnvelopeState::IDLE;
            }
            return currentValue;
        }
    }
    return 0.0f;
}

void ADSR::updateIncrements()
{
    float samplesPerMs = static_cast<float>(sampleRate) / 1000.0f;
    
    if (attackTime > 0.0f)
        attackIncrement = 1.0f / (attackTime * samplesPerMs);
    else
        attackIncrement = 1.0f; // Instant attack
    
    if (decayTime > 0.0f)
        decayIncrement = (1.0f - sustainLevel) / (decayTime * samplesPerMs);
    else
        decayIncrement = 1.0f - sustainLevel; // Instant decay
    
    if (releaseTime > 0.0f)
        releaseIncrement = sustainLevel / (releaseTime * samplesPerMs);
    else
        releaseIncrement = sustainLevel; // Instant release
}

void ADSR::saveState(juce::ValueTree& state)
{
    state.setProperty("attack", attackTime, nullptr);
    state.setProperty("decay", decayTime, nullptr);
    state.setProperty("sustain", sustainLevel, nullptr);
    state.setProperty("release", releaseTime, nullptr);
}

void ADSR::loadState(const juce::ValueTree& state)
{
    if (auto attack = state.getProperty("attack"))
        attackTime = static_cast<float>(*attack);
    
    if (auto decay = state.getProperty("decay"))
        decayTime = static_cast<float>(*decay);
    
    if (auto sustain = state.getProperty("sustain"))
        sustainLevel = static_cast<float>(*sustain);
    
    if (auto release = state.getProperty("release"))
        releaseTime = static_cast<float>(*release);
    
    updateIncrements();
}
