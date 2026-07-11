#include "Oscilloscope.h"

Oscilloscope::Oscilloscope()
{
    setOpaque(true);
}

Oscilloscope::~Oscilloscope()
{
}

void Oscilloscope::setBuffer(const juce::AudioBuffer<float>& buffer)
{
    audioBuffer = &buffer;
    repaint();
}

void Oscilloscope::setColor(juce::Colour color)
{
    waveformColor = color;
    repaint();
}

void Oscilloscope::setBackgroundColor(juce::Colour color)
{
    backgroundColor = color;
    repaint();
}

void Oscilloscope::setShowGrid(bool show)
{
    showGrid = show;
    repaint();
}

void Oscilloscope::setTriggerMode(bool enabled)
{
    triggerMode = enabled;
    repaint();
}

void Oscilloscope::setTriggerLevel(float level)
{
    triggerLevel = juce::jlimit(-1.0f, 1.0f, level);
    repaint();
}

void Oscilloscope::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColor);

    if (audioBuffer == nullptr) return;

    const int width = getWidth();
    const int height = getHeight();

    // Draw grid
    if (showGrid)
    {
        g.setColour(gridColor);
        
        // Horizontal lines
        g.drawHorizontalLine(height / 2, 0.0f, static_cast<float>(width));
        g.drawHorizontalLine(height / 4, 0.0f, static_cast<float>(width));
        g.drawHorizontalLine(height * 3 / 4, 0.0f, static_cast<float>(width));
        
        // Vertical lines
        g.drawVerticalLine(width / 2, 0.0f, static_cast<float>(height));
        g.drawVerticalLine(width / 4, 0.0f, static_cast<float>(height));
        g.drawVerticalLine(width * 3 / 4, 0.0f, static_cast<float>(height));
    }

    // Draw waveform
    g.setColour(waveformColor);
    
    const int numSamples = audioBuffer->getNumSamples();
    const int numChannels = audioBuffer->getNumChannels();
    
    if (numSamples > 0 && numChannels > 0)
    {
        // For mono, use channel 0. For stereo, average both channels.
        float centerY = static_cast<float>(height) / 2.0f;
        float scaleY = centerY * 0.9f; // Leave 10% margin
        
        juce::Path path;
        
        for (int x = 0; x < width; ++x)
        {
            // Map x to sample index
            int sampleIndex = static_cast<int>(static_cast<float>(x) / width * numSamples);
            sampleIndex = juce::jlimit(0, numSamples - 1, sampleIndex);
            
            // Get sample value (average channels if stereo)
            float sampleValue = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
            {
                sampleValue += audioBuffer->getSample(ch, sampleIndex);
            }
            sampleValue /= numChannels;
            
            // Apply trigger
            if (triggerMode)
            {
                if (sampleValue < triggerLevel)
                    continue; // Skip until trigger
            }
            
            // Map sample to y position
            float y = centerY - sampleValue * scaleY;
            
            if (x == 0)
                path.startNewSubPath(0.0f, y);
            else
                path.lineTo(static_cast<float>(x), y);
        }
        
        g.strokePath(path, juce::PathStrokeType(1.5f));
    }
}

void Oscilloscope::resized()
{
    repaint();
}
