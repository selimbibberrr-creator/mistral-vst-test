#include "PerformanceMonitor.h"
#include <chrono>

PerformanceMonitor::PerformanceMonitor()
{
    processingTimes.reserve(100); // Reserve space for 100 measurements
}

PerformanceMonitor::~PerformanceMonitor()
{
}

void PerformanceMonitor::start()
{
    startTime = std::chrono::high_resolution_clock::now();
    totalBlocksProcessed = 0;
    processingTimes.clear();
    averageProcessingTime = 0.0f;
    peakProcessingTime = 0.0f;
}

void PerformanceMonitor::stop()
{
    updateCpuUsage();
    updateMemoryUsage();
}

float PerformanceMonitor::getCpuUsage() const
{
    return cpuUsage;
}

size_t PerformanceMonitor::getMemoryUsage() const
{
    return memoryUsage;
}

float PerformanceMonitor::getAverageProcessingTime() const
{
    return averageProcessingTime;
}

float PerformanceMonitor::getPeakProcessingTime() const
{
    return peakProcessingTime;
}

void PerformanceMonitor::updateProcessingTime(float timeMs)
{
    processingTimes.push_back(timeMs);
    totalBlocksProcessed++;
    
    if (processingTimes.size() > 100)
    {
        processingTimes.erase(processingTimes.begin());
    }
    
    // Update average
    if (!processingTimes.empty())
    {
        float sum = 0.0f;
        for (float time : processingTimes)
        {
            sum += time;
        }
        averageProcessingTime = sum / processingTimes.size();
    }
    
    // Update peak
    if (timeMs > peakProcessingTime)
    {
        peakProcessingTime = timeMs;
    }
}

void PerformanceMonitor::reset()
{
    processingTimes.clear();
    totalBlocksProcessed = 0;
    averageProcessingTime = 0.0f;
    peakProcessingTime = 0.0f;
    cpuUsage = 0.0f;
    memoryUsage = 0;
}

float PerformanceMonitor::getFrameRate() const
{
    if (averageProcessingTime > 0.0f)
    {
        return 1000.0f / averageProcessingTime;
    }
    return 0.0f;
}

void PerformanceMonitor::updateCpuUsage()
{
    // Platform-specific CPU usage measurement
    // This is a simplified implementation
    
    #ifdef JUCE_WINDOWS
        // Windows implementation
        FILETIME idleTime, kernelTime, userTime;
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime))
        {
            static FILETIME prevIdleTime = idleTime;
            static FILETIME prevKernelTime = kernelTime;
            static FILETIME prevUserTime = userTime;
            
            ULARGE_INTEGER idle, kernel, user;
            idle.LowPart = idleTime.dwLowDateTime;
            idle.HighPart = idleTime.dwHighDateTime;
            kernel.LowPart = kernelTime.dwLowDateTime;
            kernel.HighPart = kernelTime.dwHighDateTime;
            user.LowPart = userTime.dwLowDateTime;
            user.HighPart = userTime.dwHighDateTime;
            
            ULARGE_INTEGER prevIdle, prevKernel, prevUser;
            prevIdle.LowPart = prevIdleTime.dwLowDateTime;
            prevIdle.HighPart = prevIdleTime.dwHighDateTime;
            prevKernel.LowPart = prevKernelTime.dwLowDateTime;
            prevKernel.HighPart = prevKernelTime.dwHighDateTime;
            prevUser.LowPart = prevUserTime.dwLowDateTime;
            prevUser.HighPart = prevUserTime.dwHighDateTime;
            
            ULARGE_INTEGER idleDiff, kernelDiff, userDiff;
            idleDiff.QuadPart = idle.QuadPart - prevIdle.QuadPart;
            kernelDiff.QuadPart = kernel.QuadPart - prevKernel.QuadPart;
            userDiff.QuadPart = user.QuadPart - prevUser.QuadPart;
            
            ULARGE_INTEGER total;
            total.QuadPart = kernelDiff.QuadPart + userDiff.QuadPart;
            
            if (total.QuadPart > 0)
            {
                cpuUsage = 100.0f - (100.0f * idleDiff.QuadPart) / total.QuadPart;
            }
            
            prevIdleTime = idleTime;
            prevKernelTime = kernelTime;
            prevUserTime = userTime;
        }
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS implementation
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0)
        {
            // This is a simplified CPU usage calculation
            // In a real implementation, we'd compare with previous values
            cpuUsage = 0.0f; // Placeholder
        }
    #else
        cpuUsage = 0.0f; // Not implemented for this platform
    #endif
}

void PerformanceMonitor::updateMemoryUsage()
{
    // Platform-specific memory usage measurement
    
    #ifdef JUCE_WINDOWS
        // Windows implementation
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo))
        {
            memoryUsage = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        }
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS implementation
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0)
        {
            memoryUsage = usage.ru_maxrss; // Resident set size
        }
    #else
        memoryUsage = 0; // Not implemented for this platform
    #endif
}
