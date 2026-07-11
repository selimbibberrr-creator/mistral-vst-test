#pragma once

#include <JuceHeader.h>
#include "StandardFilter.h"
#include "StateVariableFilter.h"

// Filter bank with 2 independent filters (A and B)
// Supports series, parallel, and mid/side routing
class FilterBank
{
public:
    enum RoutingMode
    {
        SERIES,      // A -> B
        PARALLEL,   // A + B (mixable)
        MID_SIDE,    // A (Mid), B (Side)
        FEEDBACK     // B -> A (for extreme resonance)
    };

    FilterBank();
    ~FilterBank();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Filter A (Standard Filter)
    StandardFilter& getFilterA() { return filterA; }
    const StandardFilter& getFilterA() const { return filterA; }

    // Filter B (State Variable Filter)
    StateVariableFilter& getFilterB() { return filterB; }
    const StateVariableFilter& getFilterB() const { return filterB; }

    // Routing
    void setRoutingMode(RoutingMode mode);
    void setMixRatio(float ratio); // 0.0 = 100% A, 1.0 = 100% B (for parallel mode)

    // Getters
    RoutingMode getRoutingMode() const { return routingMode; }
    float getMixRatio() const { return mixRatio; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    StandardFilter filterA;
    StateVariableFilter filterB;

    RoutingMode routingMode = SERIES;
    float mixRatio = 0.5f; // 50/50 mix for parallel mode

    // Temporary buffers for processing
    juce::AudioBuffer<float> tempBufferA;
    juce::AudioBuffer<float> tempBufferB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterBank)
};
