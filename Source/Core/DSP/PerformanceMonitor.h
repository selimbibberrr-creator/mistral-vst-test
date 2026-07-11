#pragma once

#include <JuceHeader.h>

// Performance monitoring for CPU and memory usage
class PerformanceMonitor
{
public:
    PerformanceMonitor();
    ~PerformanceMonitor();

    // Start monitoring
    void start();

    // Stop monitoring
    void stop();

    // Get CPU usage as percentage
    float getCpuUsage() const;

    // Get memory usage in bytes
    size_t getMemoryUsage() const;

    // Get average processing time per block (ms)
    float getAverageProcessingTime() const;

    // Get peak processing time (ms)
    float getPeakProcessingTime() const;

    // Update with new processing time
    void updateProcessingTime(float timeMs);

    // Reset statistics
    void reset();

    // Get frame rate (blocks per second)
    float getFrameRate() const;

private:
    // Timing
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::vector<float> processingTimes;
    size_t totalBlocksProcessed = 0;

    // CPU usage
    float cpuUsage = 0.0f;

    // Memory usage
    size_t memoryUsage = 0;

    // Statistics
    float averageProcessingTime = 0.0f;
    float peakProcessingTime = 0.0f;

    // Update CPU usage (platform-specific)
    void updateCpuUsage();

    // Update memory usage (platform-specific)
    void updateMemoryUsage();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceMonitor)
};
