#include "StereoImager.h"
#include <cmath>

StereoImager::StereoImager()
{
}

StereoImager::~StereoImager()
{
    for (auto& line : allpassLines)
        line.clear();
}

void StereoImager::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    updateAllpassFilters();
}

void StereoImager::releaseResources()
{
    for (auto& line : allpassLines)
        line.clear();
}

void StereoImager::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels < 2) return; // Need stereo

    // Process in Mid/Side
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float left = buffer.getSample(0, sample);
        float right = buffer.getSample(1, sample);
        
        // Convert to Mid/Side
        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f;
        
        // Apply width control to side channel
        float widthFactor = juce::jmap(width, 0.0f, 1.0f, 0.0f, 2.0f);
        side *= widthFactor;
        
        // Apply focus control (boost mid or side)
        float focusFactor = juce::jmap(focus, 0.0f, 1.0f, 0.5f, 2.0f);
        mid *= focusFactor;
        side *= (2.0f - focusFactor);
        
        // Convert back to stereo
        float newLeft = mid + side;
        float newRight = mid - side;
        
        // Apply phase correction
        if (phaseCorrection)
        {
            newLeft = processSample(newLeft, 0);
            newRight = processSample(newRight, 1);
        }
        
        buffer.setSample(0, sample, newLeft);
        buffer.setSample(1, sample, newRight);
    }
}

void StereoImager::setWidth(float newWidth)
{
    width = juce::jlimit(0.0f, 1.0f, newWidth);
}

void StereoImager::setFocus(float newFocus)
{
    focus = juce::jlimit(0.0f, 1.0f, newFocus);
}

void StereoImager::setPhaseCorrection(bool enabled)
{
    phaseCorrection = enabled;
    updateAllpassFilters();
}

void StereoImager::updateAllpassFilters()
{
    // Initialize allpass filters with different sizes for phase correction
    int size1 = static_cast<int>(sampleRate * 0.001f); // 1ms
    int size2 = static_cast<int>(sampleRate * 0.0015f); // 1.5ms
    
    allpassLines[0].resize(size1, 0.0f);
    allpassLines[1].resize(size2, 0.0f);
    
    allpassPositions[0] = 0;
    allpassPositions[1] = 0;
}

float StereoImager::processSample(float sample, int channel)
{
    if (!phaseCorrection) return sample;
    
    // Simple allpass filter for phase correction
    std::vector<float>& line = allpassLines[channel];
    int& pos = allpassPositions[channel];
    
    if (line.empty()) return sample;
    
    float delayed = line[pos];
    float output = -delayed + 0.7f * sample;
    
    line[pos] = output;
    pos = (pos + 1) % line.size();
    
    return output;
}

void StereoImager::saveState(juce::ValueTree& state)
{
    state.setProperty("width", width, nullptr);
    state.setProperty("focus", focus, nullptr);
    state.setProperty("phaseCorrection", phaseCorrection, nullptr);
}

void StereoImager::loadState(const juce::ValueTree& state)
{
    if (auto w = state.getProperty("width"))
        width = static_cast<float>(*w);
    
    if (auto f = state.getProperty("focus"))
        focus = static_cast<float>(*f);
    
    if (auto pc = state.getProperty("phaseCorrection"))
        phaseCorrection = static_cast<bool>(*pc);
    
    updateAllpassFilters();
}
