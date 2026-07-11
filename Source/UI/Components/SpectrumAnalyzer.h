#pragma once

#include <JuceHeader.h>

// Real-time spectrum analyzer component
class SpectrumAnalyzer : public juce::Component, private juce::Timer
{
public:
    SpectrumAnalyzer();
    ~SpectrumAnalyzer() override;

    // Set the audio buffer to analyze
    void setBuffer(const juce::AudioBuffer<float>& buffer);

    // Set display options
    void setColor(juce::Colour color);
    void setBackgroundColor(juce::Colour color);
    void setShowGrid(bool show);
    void setFrequencyRange(float minFreq, float maxFreq);
    void setNumBands(int numBands);

    // Paint
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const juce::AudioBuffer<float>* audioBuffer = nullptr;
    
    juce::Colour spectrumColor = juce::Colours::white;
    juce::Colour backgroundColor = juce::Colours::black;
    juce::Colour gridColor = juce::Colours::grey;
    
    bool showGrid = true;
    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
    int numBands = 64;
    
    // FFT and spectrum data
    std::vector<float> spectrumData;
    std::vector<float> magnitudeData;
    
    // Timer for periodic updates
    void timerCallback() override;
    
    // Calculate spectrum
    void calculateSpectrum();
    
    // Draw spectrum bars
    void drawSpectrumBars(juce::Graphics& g);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};
