#pragma once

#include <JuceHeader.h>

// Preset manager for Harde Kick Synth
class PresetManager
{
public:
    enum Genre
    {
        HARDCORE,
        HARDTECHNO,
        HARDSTYLE,
        CUSTOM
    };

    PresetManager();
    ~PresetManager();

    // Initialize with audio engine reference
    void initialize(AudioEngine* engine);

    // Preset management
    void loadPreset(const juce::String& presetName);
    void savePreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    juce::StringArray getPresetList() const;
    juce::StringArray getPresetListForGenre(Genre genre) const;

    // Factory presets
    void loadHardcorePresets();
    void loadHardtechnoPresets();
    void loadHardstylePresets();

    // Current preset
    void setCurrentPreset(const juce::String& presetName);
    const juce::String& getCurrentPreset() const { return currentPreset; }

    // Genre tags
    void setGenreTag(const juce::String& presetName, Genre genre);
    Genre getGenreTag(const juce::String& presetName) const;

    // BPM tag
    void setBPMTag(const juce::String& presetName, float bpm);
    float getBPMTag(const juce::String& presetName) const;

    // State management
    void saveState(juce::ValueTree& state);
    void loadState(const juce::ValueTree& state);

private:
    AudioEngine* audioEngine = nullptr;
    juce::String currentPreset;

    // Preset storage
    struct PresetData
    {
        juce::ValueTree state;
        Genre genre = CUSTOM;
        float bpm = 128.0f;
        juce::String description;
    };

    std::map<juce::String, PresetData> presets;

    // Load factory presets
    void loadFactoryPresets();

    // Create preset from current engine state
    PresetData createPresetFromCurrentState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
