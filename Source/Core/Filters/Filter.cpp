#include "Filter.h"

Filter::Filter()
{
}

Filter::~Filter()
{
}

void Filter::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
}

void Filter::releaseResources()
{
}

void Filter::setType(FilterType newType)
{
    type = newType;
}

void Filter::setCutoff(float frequency)
{
    cutoffFrequency = juce::jlimit(20.0f, 20000.0f, frequency);
}

void Filter::setResonance(float newResonance)
{
    resonance = juce::jlimit(0.0f, 1.0f, newResonance);
}

void Filter::setSlope(Slope newSlope)
{
    slope = newSlope;
}

void Filter::saveState(juce::ValueTree& state)
{
    state.setProperty("type", static_cast<int>(type), nullptr);
    state.setProperty("cutoff", cutoffFrequency, nullptr);
    state.setProperty("resonance", resonance, nullptr);
    state.setProperty("slope", static_cast<int>(slope), nullptr);
}

void Filter::loadState(const juce::ValueTree& state)
{
    if (auto t = state.getProperty("type"))
        type = static_cast<FilterType>(static_cast<int>(*t));
    
    if (auto cutoff = state.getProperty("cutoff"))
        cutoffFrequency = static_cast<float>(*cutoff);
    
    if (auto res = state.getProperty("resonance"))
        resonance = static_cast<float>(*res);
    
    if (auto s = state.getProperty("slope"))
        slope = static_cast<Slope>(static_cast<int>(*s));
}
