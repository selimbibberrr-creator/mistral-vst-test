#include "Chebyshev.h"
#include <cmath>

Chebyshev::Chebyshev()
{
    updateCoefficients();
}

Chebyshev::~Chebyshev()
{
}

void Chebyshev::processBlock(juce::AudioBuffer<float>& buffer, float drive, float mix)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float original = channelData[sample];
            float distorted = processSample(original, drive);
            channelData[sample] = original * (1.0f - mix) + distorted * mix;
        }
    }
}

float Chebyshev::processSample(float sample, float drive)
{
    // Scale input based on drive
    float scaledInput = sample * juce::jmap(drive, 0.0f, 1.0f, 1.0f, 5.0f);
    
    // Clamp to [-1, 1] range for Chebyshev
    scaledInput = juce::jlimit(-1.0f, 1.0f, scaledInput);
    
    // Apply Chebyshev polynomial
    return chebyshevPolynomial(scaledInput, order);
}

void Chebyshev::setOrder(int newOrder)
{
    order = juce::jlimit(2, 10, newOrder);
    updateCoefficients();
}

void Chebyshev::setParameter(int parameterIndex, float value)
{
    if (parameterIndex == 0)
    {
        order = juce::jlimit(2, 10, static_cast<int>(value));
        updateCoefficients();
    }
}

float Chebyshev::getParameter(int parameterIndex) const
{
    if (parameterIndex == 0)
        return static_cast<float>(order);
    return 0.0f;
}

void Chebyshev::updateCoefficients()
{
    // Chebyshev polynomials are defined recursively:
    // T_0(x) = 1
    // T_1(x) = x
    // T_n(x) = 2xT_{n-1}(x) - T_{n-2}(x)
    
    // We don't need to pre-compute coefficients for the recursive implementation
}

float Chebyshev::chebyshevPolynomial(float x, int n) const
{
    // Recursive implementation of Chebyshev polynomials
    if (n == 0) return 1.0f;
    if (n == 1) return x;
    
    // Use iterative approach for efficiency
    float t0 = 1.0f; // T_0(x)
    float t1 = x;    // T_1(x)
    float tn = 0.0f;
    
    for (int i = 2; i <= n; ++i)
    {
        tn = 2.0f * x * t1 - t0;
        t0 = t1;
        t1 = tn;
    }
    
    return tn;
}

void Chebyshev::saveState(juce::ValueTree& state)
{
    state.setProperty("order", order, nullptr);
}

void Chebyshev::loadState(const juce::ValueTree& state)
{
    if (auto o = state.getProperty("order"))
        order = static_cast<int>(*o);
    
    updateCoefficients();
}
