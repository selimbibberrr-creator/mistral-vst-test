#pragma once

#include <JuceHeader.h>

// Latency compensation for parallel processing paths
class LatencyCompensator
{
public:
    LatencyCompensator();
    ~LatencyCompensator();

    // Initialize with maximum latency in samples
    void prepareToPlay(double sampleRate, int samplesPerBlock, int maxLatencySamples);
    void releaseResources();

    // Add a processing path with its latency
    void addPath(juce::AudioBuffer<float>& buffer, int latencySamples);

    // Process all paths with latency compensation
    void processAllPaths();

    // Get the compensated output
    void getOutput(juce::AudioBuffer<float>& outputBuffer);

    // Reset all delay lines
    void reset();

private:
    double sampleRate = 44100.0;
    int blockSize = 512;
    int maxLatencySamples = 0;

    // Processing paths
    struct Path
    {
        juce::AudioBuffer<float> buffer;
        int latencySamples = 0;
        int delayLinePosition = 0;
        std::vector<float> delayLine;
    };

    std::vector<Path> paths;

    // Output buffer
    juce::AudioBuffer<float> outputBuffer;

    // Add delay to a path to compensate for latency
    void addDelayToPath(Path& path, int delaySamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyCompensator)
};
