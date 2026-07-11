#include "SpectrumAnalyzer.h"
#include <cmath>

SpectrumAnalyzer::SpectrumAnalyzer()
{
    setOpaque(true);
    spectrumData.resize(1024, 0.0f);
    magnitudeData.resize(numBands, 0.0f);
    startTimerHz(30); // Update 30 times per second
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
}

void SpectrumAnalyzer::setBuffer(const juce::AudioBuffer<float>& buffer)
{
    audioBuffer = &buffer;
}

void SpectrumAnalyzer::setColor(juce::Colour color)
{
    spectrumColor = color;
    repaint();
}

void SpectrumAnalyzer::setBackgroundColor(juce::Colour color)
{
    backgroundColor = color;
    repaint();
}

void SpectrumAnalyzer::setShowGrid(bool show)
{
    showGrid = show;
    repaint();
}

void SpectrumAnalyzer::setFrequencyRange(float minFreq, float maxFreq)
{
    minFrequency = juce::jmax(10.0f, minFreq);
    maxFrequency = juce::jmin(22000.0f, maxFreq);
    repaint();
}

void SpectrumAnalyzer::setNumBands(int numBands)
{
    this->numBands = juce::jlimit(8, 128, numBands);
    magnitudeData.resize(this->numBands, 0.0f);
    repaint();
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColor);

    const int width = getWidth();
    const int height = getHeight();

    // Draw grid
    if (showGrid)
    {
        g.setColour(gridColor);
        
        // Frequency grid lines
        float logMin = std::log10(minFrequency);
        float logMax = std::log10(maxFrequency);
        float logRange = logMax - logMin;
        
        for (int i = 0; i <= 10; ++i)
        {
            float freq = std::pow(10.0f, logMin + logRange * i / 10.0f);
            int x = static_cast<int>(width * i / 10.0f);
            g.drawVerticalLine(x, 0.0f, static_cast<float>(height));
            
            // Draw frequency label
            g.setFont(juce::Font(10.0f));
            g.drawText(juce::String(static_cast<int>(freq)) + "Hz", 
                      x + 2, height - 20, 50, 15, 
                      juce::Justification::bottomLeft);
        }
        
        // dB grid lines
        for (int i = 0; i <= 6; ++i)
        {
            float db = -60.0f + i * 20.0f;
            int y = static_cast<int>(height * (1.0f - i / 6.0f));
            g.drawHorizontalLine(y, 0.0f, static_cast<float>(width));
            
            // Draw dB label
            g.drawText(juce::String(static_cast<int>(db)) + "dB", 
                      2, y - 2, 40, 15, 
                      juce::Justification::centredRight);
        }
    }
    
    // Draw spectrum
    drawSpectrumBars(g);
}

void SpectrumAnalyzer::resized()
{
    repaint();
}

void SpectrumAnalyzer::timerCallback()
{
    if (audioBuffer != nullptr)
    {
        calculateSpectrum();
        repaint();
    }
}

void SpectrumAnalyzer::calculateSpectrum()
{
    if (audioBuffer == nullptr || audioBuffer->getNumSamples() == 0)
        return;
    
    const int bufferSize = audioBuffer->getNumSamples();
    const int numChannels = audioBuffer->getNumChannels();
    
    // Simple FFT-like analysis (in a real implementation, use juce::dsp::FFT)
    // For now, we'll do a simple frequency analysis
    
    // Average all channels
    std::vector<float> monoBuffer(bufferSize, 0.0f);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            monoBuffer[i] += audioBuffer->getSample(ch, i);
        }
    }
    
    for (int i = 0; i < bufferSize; ++i)
    {
        monoBuffer[i] /= numChannels;
    }
    
    // Simple frequency analysis using Goertzel algorithm for each band
    // This is a simplified approach - in a real implementation, use FFT
    
    float sampleRate = 44100.0f; // Assume 44.1kHz
    
    for (int band = 0; band < numBands; ++band)
    {
        // Calculate frequency for this band (logarithmic scale)
        float minLog = std::log10(minFrequency);
        float maxLog = std::log10(maxFrequency);
        float bandLog = minLog + (maxLog - minLog) * band / (numBands - 1);
        float bandFreq = std::pow(10.0f, bandLog);
        
        // Simple bandpass filter approach
        float magnitude = 0.0f;
        
        for (int i = 0; i < bufferSize; ++i)
        {
            // Simple bandpass using resonance
            float omega = 2.0f * juce::MathConstants<float>::pi * bandFreq / sampleRate;
            float q = 10.0f; // High Q for narrow band
            float alpha = std::sin(omega) / (2.0f * q);
            float a0 = 1.0f + alpha;
            
            // This is a placeholder - in a real implementation, we'd use proper filtering
            magnitude += std::abs(monoBuffer[i]) * 0.1f;
        }
        
        magnitude /= bufferSize;
        
        // Convert to dB
        magnitude = 20.0f * std::log10(magnitude + 0.00001f);
        magnitude = juce::jlimit(-60.0f, 0.0f, magnitude);
        
        magnitudeData[band] = magnitude;
    }
}

void SpectrumAnalyzer::drawSpectrumBars(juce::Graphics& g)
{
    const int width = getWidth();
    const int height = getHeight();
    
    float barWidth = static_cast<float>(width) / numBands;
    
    g.setColour(spectrumColor);
    
    for (int band = 0; band < numBands; ++band)
    {
        float magnitude = magnitudeData[band];
        
        // Convert dB to height (0dB = top, -60dB = bottom)
        float normalizedMag = juce::jmap(magnitude, -60.0f, 0.0f, 0.0f, 1.0f);
        int barHeight = static_cast<int>(height * normalizedMag);
        
        int x = static_cast<int>(band * barWidth);
        int y = height - barHeight;
        
        g.fillRect(x, y, static_cast<int>(barWidth) - 1, barHeight);
    }
}
