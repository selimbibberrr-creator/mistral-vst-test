#include "ADSREditor.h"

ADSREditor::ADSREditor()
{
    setOpaque(true);
    
    // Initialize default points
    points[ATTACK].x = 0.0f;
    points[ATTACK].y = 1.0f;
    points[DECAY].x = 0.3f;
    points[DECAY].y = 0.5f;
    points[SUSTAIN].x = 0.6f;
    points[SUSTAIN].y = 0.3f;
    points[RELEASE].x = 1.0f;
    points[RELEASE].y = 0.0f;
}

ADSREditor::~ADSREditor()
{
}

void ADSREditor::setADSR(ADSR* newADSR)
{
    adsr = newADSR;
    updatePoints();
    repaint();
}

void ADSREditor::setColor(juce::Colour color)
{
    curveColor = color;
    repaint();
}

void ADSREditor::setBackgroundColor(juce::Colour color)
{
    backgroundColor = color;
    repaint();
}

void ADSREditor::setGridColor(juce::Colour color)
{
    gridColor = color;
    repaint();
}

void ADSREditor::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColor);

    drawGrid(g);
    drawEnvelope(g);
    drawPoints(g);
}

void ADSREditor::resized()
{
    repaint();
}

void ADSREditor::mouseDown(const juce::MouseEvent& event)
{
    if (adsr == nullptr) return;
    
    // Check if we clicked on a point
    for (int i = 0; i < NUM_POINTS; ++i)
    {
        juce::Point<int> screenPoint = normalizedToScreen({points[i].x, points[i].y});
        float distance = std::sqrt(std::pow(event.x - screenPoint.x, 2) + 
                                   std::pow(event.y - screenPoint.y, 2));
        
        if (distance < 10.0f) // 10 pixel radius
        {
            activePoint = i;
            break;
        }
    }
}

void ADSREditor::mouseDrag(const juce::MouseEvent& event)
{
    if (activePoint == -1) return;
    
    juce::Point<float> normalized = screenToNormalized({event.x, event.y});
    
    // Constrain points
    switch (activePoint)
    {
        case ATTACK:
            normalized.x = 0.0f; // Attack always starts at 0
            normalized.y = juce::jlimit(0.0f, 1.0f, normalized.y);
            break;
        case DECAY:
            normalized.x = juce::jlimit(points[ATTACK].x, points[SUSTAIN].x, normalized.x);
            normalized.y = juce::jlimit(0.0f, 1.0f, normalized.y);
            break;
        case SUSTAIN:
            normalized.x = juce::jlimit(points[DECAY].x, points[RELEASE].x, normalized.x);
            normalized.y = juce::jlimit(0.0f, 1.0f, normalized.y);
            break;
        case RELEASE:
            normalized.x = 1.0f; // Release always ends at 1
            normalized.y = juce::jlimit(0.0f, 1.0f, normalized.y);
            break;
    }
    
    points[activePoint] = normalized;
    updateADSR();
    repaint();
}

void ADSREditor::mouseUp(const juce::MouseEvent& event)
{
    activePoint = -1;
}

void ADSREditor::updatePoints()
{
    if (adsr == nullptr) return;
    
    // Get ADSR parameters
    float attack = adsr->getAttack();
    float decay = adsr->getDecay();
    float sustain = adsr->getSustain();
    float release = adsr->getRelease();
    
    // Normalize times to 0-1 range (assuming max 2000ms for display)
    float maxTime = 2000.0f;
    points[ATTACK].x = 0.0f;
    points[ATTACK].y = 1.0f;
    points[DECAY].x = juce::jlimit(0.0f, 1.0f, attack / maxTime);
    points[DECAY].y = juce::jlimit(0.0f, 1.0f, 1.0f - sustain);
    points[SUSTAIN].x = juce::jlimit(points[DECAY].x, 1.0f, (attack + decay) / maxTime);
    points[SUSTAIN].y = juce::jlimit(0.0f, 1.0f, sustain);
    points[RELEASE].x = 1.0f;
    points[RELEASE].y = 0.0f;
}

void ADSREditor::updateADSR()
{
    if (adsr == nullptr) return;
    
    // Convert points back to ADSR parameters
    float maxTime = 2000.0f;
    
    float attack = points[DECAY].x * maxTime;
    float decay = (points[SUSTAIN].x - points[DECAY].x) * maxTime;
    float sustain = points[SUSTAIN].y;
    float release = (1.0f - points[SUSTAIN].x) * maxTime;
    
    adsr->setAttack(attack);
    adsr->setDecay(decay);
    adsr->setSustain(sustain);
    adsr->setRelease(release);
}

void ADSREditor::drawEnvelope(juce::Graphics& g)
{
    g.setColour(curveColor);
    
    juce::Path path;
    
    // Start at attack point
    juce::Point<int> startPoint = normalizedToScreen({points[ATTACK].x, points[ATTACK].y});
    path.startNewSubPath(startPoint.x, startPoint.y);
    
    // Line to decay point
    juce::Point<int> decayPoint = normalizedToScreen({points[DECAY].x, points[DECAY].y});
    path.lineTo(decayPoint.x, decayPoint.y);
    
    // Line to sustain point
    juce::Point<int> sustainPoint = normalizedToScreen({points[SUSTAIN].x, points[SUSTAIN].y});
    path.lineTo(sustainPoint.x, sustainPoint.y);
    
    // Line to release point
    juce::Point<int> releasePoint = normalizedToScreen({points[RELEASE].x, points[RELEASE].y});
    path.lineTo(releasePoint.x, releasePoint.y);
    
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void ADSREditor::drawGrid(juce::Graphics& g)
{
    g.setColour(gridColor);
    
    const int width = getWidth();
    const int height = getHeight();
    
    // Horizontal lines (level)
    for (int i = 0; i <= 4; ++i)
    {
        float y = static_cast<float>(height) * (1.0f - i / 4.0f);
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(width));
        
        // Draw label
        g.setFont(juce::Font(10.0f));
        float value = 1.0f - i / 4.0f;
        g.drawText(juce::String(value, 2), 
                  2, static_cast<int>(y) - 5, 40, 10, 
                  juce::Justification::centredRight);
    }
    
    // Vertical lines (time)
    for (int i = 0; i <= 4; ++i)
    {
        float x = static_cast<float>(width) * i / 4.0f;
        g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
        
        // Draw label
        g.setFont(juce::Font(10.0f));
        float time = i * 500.0f; // 0, 500, 1000, 1500, 2000 ms
        g.drawText(juce::String(static_cast<int>(time)) + "ms", 
                  static_cast<int>(x) + 2, height - 15, 50, 10, 
                  juce::Justification::bottomLeft);
    }
}

void ADSREditor::drawPoints(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    
    for (int i = 0; i < NUM_POINTS; ++i)
    {
        juce::Point<int> screenPoint = normalizedToScreen({points[i].x, points[i].y});
        
        // Draw circle
        g.fillEllipse(screenPoint.x - 5, screenPoint.y - 5, 10, 10);
        
        // Draw label
        g.setFont(juce::Font(10.0f));
        const char* labels[] = {"A", "D", "S", "R"};
        g.drawText(juce::String(labels[i]), 
                  screenPoint.x + 8, screenPoint.y - 8, 20, 15, 
                  juce::Justification::centredLeft);
    }
}

juce::Point<float> ADSREditor::screenToNormalized(const juce::Point<int>& screenPoint)
{
    return {
        juce::jlimit(0.0f, 1.0f, static_cast<float>(screenPoint.x) / getWidth()),
        juce::jlimit(0.0f, 1.0f, 1.0f - static_cast<float>(screenPoint.y) / getHeight())
    };
}

juce::Point<int> ADSREditor::normalizedToScreen(const juce::Point<float>& normalizedPoint)
{
    return {
        static_cast<int>(normalizedPoint.x * getWidth()),
        static_cast<int>((1.0f - normalizedPoint.y) * getHeight())
    };
}
