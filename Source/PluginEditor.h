#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class HardeKickSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    HardeKickSynthAudioProcessorEditor(HardeKickSynthAudioProcessor&);
    ~HardeKickSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HardeKickSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardeKickSynthAudioProcessorEditor)
};
