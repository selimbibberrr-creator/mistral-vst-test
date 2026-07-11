#pragma once

#include <JuceHeader.h>
#include "Components/Oscilloscope.h"
#include "Components/SpectrumAnalyzer.h"
#include "Components/ADSREditor.h"

// Main window for the Harde Kick Synth plugin
class MainWindow : public juce::Component
{
public:
    MainWindow(AudioEngine& engine);
    ~MainWindow() override;

    // Paint
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    AudioEngine& audioEngine;

    // UI Components
    Oscilloscope oscilloscope;
    SpectrumAnalyzer spectrumAnalyzer;
    ADSREditor adsrEditor;

    // Sliders for parameters
    juce::Slider osc1LevelSlider;
    juce::Slider osc1PitchSlider;
    juce::Slider osc2LevelSlider;
    juce::Slider filterCutoffSlider;
    juce::Slider distortionDriveSlider;
    juce::Slider masterVolumeSlider;

    // Labels
    juce::Label osc1LevelLabel;
    juce::Label osc1PitchLabel;
    juce::Label osc2LevelLabel;
    juce::Label filterCutoffLabel;
    juce::Label distortionDriveLabel;
    juce::Label masterVolumeLabel;

    // Combo boxes for selections
    juce::ComboBox osc1WaveformBox;
    juce::ComboBox filterTypeBox;
    juce::ComboBox distortionAlgorithmBox;

    // Buttons
    juce::TextButton presetButton;
    juce::TextButton saveButton;

    // Setup UI components
    void setupSliders();
    void setupLabels();
    void setupComboBoxes();
    void setupButtons();

    // Slider listeners
    void sliderValueChanged(juce::Slider* slider);

    // Combo box listeners
    void comboBoxChanged(juce::ComboBox* comboBox);

    // Button listeners
    void buttonClicked(juce::Button* button);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
