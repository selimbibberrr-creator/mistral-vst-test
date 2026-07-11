#pragma once

#include <JuceHeader.h>

// Reverb effect with Hall, Room, and Spring modes
class Reverb
{
public:
    enum ReverbType
    {
        HALL,
        ROOM,
        SPRING
    };

    Reverb();
    ~Reverb();

    // Lifecycle
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Setters
    void setType(ReverbType type);
    void setDecay(float decay); // 0.1 to 10.0 seconds
    void setPreDelay(float preDelay); // 0 to 500ms
    void setSize(float size); // 0.0 to 1.0
    void setDamping(float damping); // 0.0 to 1.0 (high frequency damping)
    void setMix(float mix); // 0.0 to 1.0 (dry/wet)

    // Getters
    ReverbType getType() const { return type; }
    float getDecay() const { return decay; }
    float getPreDelay() const { return preDelay; }
    float getSize() const { return size; }
    float getDamping() const { return damping; }
    float getMix() const { return mix; }

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    double sampleRate = 44100.0;
    int blockSize = 512;

    ReverbType type = HALL;
    float decay = 2.0f; // seconds
    float preDelay = 50.0f; // ms
    float size = 0.5f; // 0.0 to 1.0
    float damping = 0.5f; // 0.0 to 1.0
    float mix = 0.3f; // 0.0 to 1.0

    // Internal state
    std::vector<float> delayLines[4]; // 4 delay lines for stereo
    int delayLinePositions[4] = {0, 0, 0, 0};
    std::vector<float> allpassLines[2]; // 2 allpass filters
    int allpassPositions[2] = {0, 0};

    // Filter state for damping
    float dampingFilterState[2][2] = {{0.0f, 0.0f}, {0.0f, 0.0f}};

    // Pre-delay buffer
    juce::AudioBuffer<float> preDelayBuffer;
    int preDelayPosition = 0;

    // Update delay line sizes based on parameters
    void updateDelayLines();

    // Process a single sample
    float processSample(float sample, int channel);

    // Generate a random number
    float random() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};
