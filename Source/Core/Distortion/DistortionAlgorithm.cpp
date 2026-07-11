#include "DistortionAlgorithm.h"

DistortionAlgorithm::DistortionAlgorithm()
{
}

DistortionAlgorithm::~DistortionAlgorithm()
{
}

void DistortionAlgorithm::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
}

void DistortionAlgorithm::releaseResources()
{
}

void DistortionAlgorithm::saveState(juce::ValueTree& /*state*/)
{
}

void DistortionAlgorithm::loadState(const juce::ValueTree& /*state*/)
{
}
