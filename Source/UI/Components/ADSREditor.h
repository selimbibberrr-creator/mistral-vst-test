#pragma once

#include <JuceHeader.h>
#include "../../Core/Envelopes/ADSR.h"

// ADSR envelope editor component
class ADSREditor : public juce::Component
{
public:
    ADSREditor();
    ~ADSREditor() override;

    // Set the ADSR to edit
    void setADSR(ADSR* adsr);

    // Set color scheme
    void setColor(juce::Colour color);
    void setBackgroundColor(juce::Colour color);
    void setGridColor(juce::Colour color);

    // Paint
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Mouse interaction
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    ADSR* adsr = nullptr;
    
    juce::Colour curveColor = juce::Colours::white;
    juce::Colour backgroundColor = juce::Colours::black;
    juce::Colour gridColor = juce::Colours::grey;
    
    // Envelope points for editing
    enum EnvelopePoint
    {
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE,
        NUM_POINTS
    };
    
    // Point positions (normalized 0-1)
    struct Point
    {
        float x = 0.0f;
        float y = 0.0f;
    };
    
    Point points[NUM_POINTS];
    
    // Active point for dragging
    int activePoint = -1;
    
    // Update point positions from ADSR
    void updatePoints();
    
    // Update ADSR from point positions
    void updateADSR();
    
    // Draw the envelope curve
    void drawEnvelope(juce::Graphics& g);
    
    // Draw the grid
    void drawGrid(juce::Graphics& g);
    
    // Draw the points
    void drawPoints(juce::Graphics& g);
    
    // Convert screen coordinates to normalized coordinates
    juce::Point<float> screenToNormalized(const juce::Point<int>& screenPoint);
    
    // Convert normalized coordinates to screen coordinates
    juce::Point<int> normalizedToScreen(const juce::Point<float>& normalizedPoint);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSREditor)
};
