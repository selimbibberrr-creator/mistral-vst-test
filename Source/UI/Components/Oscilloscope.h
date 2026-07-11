#pragma once

#include <JuceHeader.h>

// Real-time oscilloscope component
class Oscilloscope : public juce::Component
{
public:
    Oscilloscope();
    ~Oscilloscope() override;

    // Set the audio buffer to visualize
    void setBuffer(const juce::AudioBuffer<float>& buffer);

    // Set color scheme
    void setColor(juce::Colour color);
    void setBackgroundColor(juce::Colour color);

    // Set display options
    void setShowGrid(bool show);
    void setTriggerMode(bool enabled);
    void setTriggerLevel(float level);

    // Paint
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const juce::AudioBuffer<float>* audioBuffer = nullptr;
    
    juce::Colour waveformColor = juce::Colours::white;
    juce::Colour backgroundColor = juce::Colours::black;
    juce::Colour gridColor = juce::Colours::grey;
    
    bool showGrid = true;
    bool triggerMode = false;
    float triggerLevel = 0.0f;
    
    // Waveform data for drawing
    juce::Path waveformPath;
    
    // Update the waveform path
    void updateWaveformPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscilloscope)
};
