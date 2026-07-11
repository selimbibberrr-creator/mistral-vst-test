#include "MainWindow.h"

MainWindow::MainWindow(AudioEngine& engine) : audioEngine(engine)
{
    // Setup UI components
    setupSliders();
    setupLabels();
    setupComboBoxes();
    setupButtons();

    // Set ADSR editor
    adsrEditor.setADSR(&audioEngine.getADSR_Osc1());

    // Add components
    addAndMakeVisible(oscilloscope);
    addAndMakeVisible(spectrumAnalyzer);
    addAndMakeVisible(adsrEditor);
    
    addAndMakeVisible(osc1LevelSlider);
    addAndMakeVisible(osc1PitchSlider);
    addAndMakeVisible(osc2LevelSlider);
    addAndMakeVisible(filterCutoffSlider);
    addAndMakeVisible(distortionDriveSlider);
    addAndMakeVisible(masterVolumeSlider);
    
    addAndMakeVisible(osc1LevelLabel);
    addAndMakeVisible(osc1PitchLabel);
    addAndMakeVisible(osc2LevelLabel);
    addAndMakeVisible(filterCutoffLabel);
    addAndMakeVisible(distortionDriveLabel);
    addAndMakeVisible(masterVolumeLabel);
    
    addAndMakeVisible(osc1WaveformBox);
    addAndMakeVisible(filterTypeBox);
    addAndMakeVisible(distortionAlgorithmBox);
    
    addAndMakeVisible(presetButton);
    addAndMakeVisible(saveButton);

    // Set initial sizes (will be updated in resized)
    setSize(1200, 800);
}

MainWindow::~MainWindow()
{
}

void MainWindow::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    // Draw section headers
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    
    g.drawText("Oscillators", 20, 20, 200, 20, juce::Justification::left);
    g.drawText("Filters", 20, 180, 200, 20, juce::Justification::left);
    g.drawText("Distortion", 20, 260, 200, 20, juce::Justification::left);
    g.drawText("Envelope", 20, 340, 200, 20, juce::Justification::left);
    g.drawText("Visual Feedback", 20, 420, 200, 20, juce::Justification::left);
    g.drawText("Master", 20, 600, 200, 20, juce::Justification::left);
}

void MainWindow::resized()
{
    const int width = getWidth();
    const int height = getHeight();
    
    // Visual feedback section (top)
    int visualHeight = height / 3;
    oscilloscope.setBounds(20, 440, width / 2 - 30, visualHeight / 2 - 10);
    spectrumAnalyzer.setBounds(width / 2 + 10, 440, width / 2 - 30, visualHeight / 2 - 10);
    adsrEditor.setBounds(20, 440 + visualHeight / 2, width - 40, visualHeight / 2 - 10);
    
    // Oscillator controls
    int controlWidth = 120;
    int controlHeight = 20;
    int controlSpacing = 30;
    
    osc1LevelLabel.setBounds(20, 40, controlWidth, controlHeight);
    osc1LevelSlider.setBounds(20, 60, controlWidth, controlHeight);
    
    osc1PitchLabel.setBounds(20 + controlWidth + 20, 40, controlWidth, controlHeight);
    osc1PitchSlider.setBounds(20 + controlWidth + 20, 60, controlWidth, controlHeight);
    
    osc2LevelLabel.setBounds(20 + (controlWidth + 20) * 2, 40, controlWidth, controlHeight);
    osc2LevelSlider.setBounds(20 + (controlWidth + 20) * 2, 60, controlWidth, controlHeight);
    
    osc1WaveformBox.setBounds(20 + (controlWidth + 20) * 3, 40, controlWidth, controlHeight);
    
    // Filter controls
    filterCutoffLabel.setBounds(20, 200, controlWidth, controlHeight);
    filterCutoffSlider.setBounds(20, 220, controlWidth, controlHeight);
    filterTypeBox.setBounds(20 + controlWidth + 20, 200, controlWidth, controlHeight);
    
    // Distortion controls
    distortionDriveLabel.setBounds(20, 280, controlWidth, controlHeight);
    distortionDriveSlider.setBounds(20, 300, controlWidth, controlHeight);
    distortionAlgorithmBox.setBounds(20 + controlWidth + 20, 280, controlWidth, controlHeight);
    
    // Master controls
    masterVolumeLabel.setBounds(20, 620, controlWidth, controlHeight);
    masterVolumeSlider.setBounds(20, 640, controlWidth, controlHeight);
    
    // Buttons
    presetButton.setBounds(width - 200, height - 40, 90, 30);
    saveButton.setBounds(width - 100, height - 40, 90, 30);
}

void MainWindow::setupSliders()
{
    // Oscillator 1 Level
    osc1LevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc1LevelSlider.setRange(0.0, 1.0, 0.01);
    osc1LevelSlider.setValue(1.0, juce::dontSendNotification);
    osc1LevelSlider.addListener(this);
    osc1LevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

    // Oscillator 1 Pitch
    osc1PitchSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc1PitchSlider.setRange(20.0, 200.0, 1.0);
    osc1PitchSlider.setValue(60.0, juce::dontSendNotification);
    osc1PitchSlider.addListener(this);
    osc1PitchSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

    // Oscillator 2 Level
    osc2LevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc2LevelSlider.setRange(0.0, 1.0, 0.01);
    osc2LevelSlider.setValue(1.0, juce::dontSendNotification);
    osc2LevelSlider.addListener(this);
    osc2LevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

    // Filter Cutoff
    filterCutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    filterCutoffSlider.setRange(20.0, 20000.0, 10.0);
    filterCutoffSlider.setValue(1000.0, juce::dontSendNotification);
    filterCutoffSlider.addListener(this);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterCutoffSlider.setSkewFactorFromMidPoint(1000.0); // Logarithmic scale

    // Distortion Drive
    distortionDriveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    distortionDriveSlider.setRange(0.0, 1.0, 0.01);
    distortionDriveSlider.setValue(0.5, juce::dontSendNotification);
    distortionDriveSlider.addListener(this);
    distortionDriveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

    // Master Volume
    masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(1.0, juce::dontSendNotification);
    masterVolumeSlider.addListener(this);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
}

void MainWindow::setupLabels()
{
    osc1LevelLabel.setText("Osc1 Level", juce::dontSendNotification);
    osc1LevelLabel.setFont(juce::Font(12.0f));
    osc1LevelLabel.setJustificationType(juce::Justification::centredLeft);

    osc1PitchLabel.setText("Osc1 Pitch", juce::dontSendNotification);
    osc1PitchLabel.setFont(juce::Font(12.0f));
    osc1PitchLabel.setJustificationType(juce::Justification::centredLeft);

    osc2LevelLabel.setText("Osc2 Level", juce::dontSendNotification);
    osc2LevelLabel.setFont(juce::Font(12.0f));
    osc2LevelLabel.setJustificationType(juce::Justification::centredLeft);

    filterCutoffLabel.setText("Filter Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setFont(juce::Font(12.0f));
    filterCutoffLabel.setJustificationType(juce::Justification::centredLeft);

    distortionDriveLabel.setText("Distortion Drive", juce::dontSendNotification);
    distortionDriveLabel.setFont(juce::Font(12.0f));
    distortionDriveLabel.setJustificationType(juce::Justification::centredLeft);

    masterVolumeLabel.setText("Master Volume", juce::dontSendNotification);
    masterVolumeLabel.setFont(juce::Font(12.0f));
    masterVolumeLabel.setJustificationType(juce::Justification::centredLeft);
}

void MainWindow::setupComboBoxes()
{
    // Oscillator 1 Waveform
    osc1WaveformBox.addItem("Sine", 1);
    osc1WaveformBox.addItem("Square", 2);
    osc1WaveformBox.addItem("Saw", 3);
    osc1WaveformBox.addItem("Triangle", 4);
    osc1WaveformBox.setSelectedId(1, juce::dontSendNotification);
    osc1WaveformBox.addListener(this);

    // Filter Type
    filterTypeBox.addItem("Low Pass", 1);
    filterTypeBox.addItem("High Pass", 2);
    filterTypeBox.addItem("Band Pass", 3);
    filterTypeBox.addItem("Notch", 4);
    filterTypeBox.setSelectedId(1, juce::dontSendNotification);
    filterTypeBox.addListener(this);

    // Distortion Algorithm
    distortionAlgorithmBox.addItem("Soft Clip", 1);
    distortionAlgorithmBox.addItem("Hard Clip", 2);
    distortionAlgorithmBox.addItem("Chebyshev", 3);
    distortionAlgorithmBox.addItem("Bitcrush", 4);
    distortionAlgorithmBox.addItem("Tape", 5);
    distortionAlgorithmBox.addItem("Foldback", 6);
    distortionAlgorithmBox.setSelectedId(1, juce::dontSendNotification);
    distortionAlgorithmBox.addListener(this);
}

void MainWindow::setupButtons()
{
    presetButton.setButtonText("Load Preset");
    presetButton.addListener(this);

    saveButton.setButtonText("Save Preset");
    saveButton.addListener(this);
}

void MainWindow::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &osc1LevelSlider)
    {
        audioEngine.getOscillator1().setLevel(static_cast<float>(slider->getValue()));
    }
    else if (slider == &osc1PitchSlider)
    {
        audioEngine.getOscillator1().setFrequency(static_cast<float>(slider->getValue()));
    }
    else if (slider == &osc2LevelSlider)
    {
        audioEngine.getOscillator2().setLevel(static_cast<float>(slider->getValue()));
    }
    else if (slider == &filterCutoffSlider)
    {
        audioEngine.getFilterBank().getFilterA().setCutoff(static_cast<float>(slider->getValue()));
    }
    else if (slider == &distortionDriveSlider)
    {
        audioEngine.getDistortionUnit().setDrive(static_cast<float>(slider->getValue()));
    }
    else if (slider == &masterVolumeSlider)
    {
        audioEngine.getMixer().setMasterLevel(static_cast<float>(slider->getValue()));
    }
}

void MainWindow::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &osc1WaveformBox)
    {
        int waveform = comboBox->getSelectedId();
        audioEngine.getOscillator1().setWaveform(static_cast<Oscillator::WaveformType>(waveform - 1));
    }
    else if (comboBox == &filterTypeBox)
    {
        int type = comboBox->getSelectedId();
        audioEngine.getFilterBank().getFilterA().setType(static_cast<Filter::FilterType>(type - 1));
    }
    else if (comboBox == &distortionAlgorithmBox)
    {
        int algorithm = comboBox->getSelectedId();
        audioEngine.getDistortionUnit().setAlgorithm(static_cast<DistortionUnit::AlgorithmType>(algorithm - 1));
    }
}

void MainWindow::buttonClicked(juce::Button* button)
{
    if (button == &presetButton)
    {
        // Show preset menu
        juce::PopupMenu menu;
        menu.addItem("Hardcore Gabber Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardcore Gabber Kick"); });
        menu.addItem("Hardcore Barking Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardcore Barking Kick"); });
        menu.addItem("Hardtechno Knocking Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardtechno Knocking Kick"); });
        menu.addItem("Hardtechno Deep Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardtechno Deep Kick"); });
        menu.addItem("Hardstyle Bonk Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardstyle Bonk Kick"); });
        menu.addItem("Hardstyle Tail Kick", [this] { audioEngine.getPresetManager().loadPreset("Hardstyle Tail Kick"); });
        
        menu.showMenuAsync(juce::PopupMenu::Options().withParentComponent(this));
    }
    else if (button == &saveButton)
    {
        // Save current state as preset
        audioEngine.getPresetManager().savePreset("User Preset " + juce::String(audioEngine.getPresetManager().getPresetList().size() + 1));
    }
}
