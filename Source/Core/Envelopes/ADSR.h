#pragma once

#include <JuceHeader.h>

class ADSR
{
public:
    ADSR();
    ~ADSR();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Envelope control
    void noteOn();
    void noteOff();
    void reset();

    // Setters
    void setAttack(float timeMs);
    void setDecay(float timeMs);
    void setSustain(float level);
    void setRelease(float timeMs);

    // Getters
    float getAttack() const { return attackTime; }
    float getDecay() const { return decayTime; }
    float getSustain() const { return sustainLevel; }
    float getRelease() const { return releaseTime; }

    // Get next sample value (0.0 to 1.0)
    float getNextSample();

    // Check if envelope is active
    bool isActive() const { return state != EnvelopeState::IDLE; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    enum class EnvelopeState
    {
        IDLE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    double sampleRate = 44100.0;
    
    // Envelope parameters (in milliseconds)
    float attackTime = 10.0f;   // 10ms
    float decayTime = 100.0f;   // 100ms
    float sustainLevel = 0.5f; // 50%
    float releaseTime = 200.0f; // 200ms

    // Internal state
    EnvelopeState state = EnvelopeState::IDLE;
    float currentValue = 0.0f;
    float attackIncrement = 0.0f;
    float decayIncrement = 0.0f;
    float releaseIncrement = 0.0f;
    int samplesUntilNextStage = 0;

    // Calculate increments
    void updateIncrements();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSR)
};
