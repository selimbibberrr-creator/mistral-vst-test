#include "PluginEditor.h"

HardeKickSynthAudioProcessorEditor::HardeKickSynthAudioProcessorEditor(HardeKickSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1200, 800);
    setResizable(true, true);
}

HardeKickSynthAudioProcessorEditor::~HardeKickSynthAudioProcessorEditor()
{
}

void HardeKickSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("Harde Kick Synth", getLocalBounds(), juce::Justification::centredTop, true);
    
    // Draw placeholder for UI components
    g.setColour(juce::Colours::lightgrey);
    g.drawText("UI Components will be added here", getLocalBounds(), juce::Justification::centred, true);
}

void HardeKickSynthAudioProcessorEditor::resized()
{
}
