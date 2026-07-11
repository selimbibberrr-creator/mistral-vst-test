#include "PluginEditor.h"

HardeKickSynthAudioProcessorEditor::HardeKickSynthAudioProcessorEditor(HardeKickSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mainWindow(p.getAudioEngine())
{
    setSize(1200, 800);
    setResizable(true, true);
    setResizableBoundsEnabled(true);
    
    addAndMakeVisible(mainWindow);
}

HardeKickSynthAudioProcessorEditor::~HardeKickSynthAudioProcessorEditor()
{
}

void HardeKickSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background is handled by MainWindow
}

void HardeKickSynthAudioProcessorEditor::resized()
{
    mainWindow.setBounds(getLocalBounds());
}
