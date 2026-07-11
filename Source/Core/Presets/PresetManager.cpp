#include "PresetManager.h"
#include "../AudioEngine.h"

PresetManager::PresetManager()
{
    loadFactoryPresets();
}

PresetManager::~PresetManager()
{
}

void PresetManager::initialize(AudioEngine* engine)
{
    audioEngine = engine;
}

void PresetManager::loadPreset(const juce::String& presetName)
{
    if (presets.find(presetName) != presets.end())
    {
        currentPreset = presetName;
        if (audioEngine != nullptr)
        {
            audioEngine->loadState(presets[presetName].state);
        }
    }
}

void PresetManager::savePreset(const juce::String& presetName)
{
    if (audioEngine != nullptr)
    {
        PresetData preset = createPresetFromCurrentState();
        preset.description = "User preset: " + presetName;
        presets[presetName] = preset;
        currentPreset = presetName;
    }
}

void PresetManager::deletePreset(const juce::String& presetName)
{
    presets.erase(presetName);
    if (currentPreset == presetName)
        currentPreset = "";
}

juce::StringArray PresetManager::getPresetList() const
{
    juce::StringArray list;
    for (const auto& pair : presets)
    {
        list.add(pair.first);
    }
    return list;
}

juce::StringArray PresetManager::getPresetListForGenre(Genre genre) const
{
    juce::StringArray list;
    for (const auto& pair : presets)
    {
        if (pair.second.genre == genre)
            list.add(pair.first);
    }
    return list;
}

void PresetManager::setCurrentPreset(const juce::String& presetName)
{
    currentPreset = presetName;
}

void PresetManager::setGenreTag(const juce::String& presetName, Genre genre)
{
    if (presets.find(presetName) != presets.end())
    {
        presets[presetName].genre = genre;
    }
}

PresetManager::Genre PresetManager::getGenreTag(const juce::String& presetName) const
{
    auto it = presets.find(presetName);
    if (it != presets.end())
        return it->second.genre;
    return CUSTOM;
}

void PresetManager::setBPMTag(const juce::String& presetName, float bpm)
{
    if (presets.find(presetName) != presets.end())
    {
        presets[presetName].bpm = bpm;
    }
}

float PresetManager::getBPMTag(const juce::String& presetName) const
{
    auto it = presets.find(presetName);
    if (it != presets.end())
        return it->second.bpm;
    return 128.0f;
}

void PresetManager::loadFactoryPresets()
{
    loadHardcorePresets();
    loadHardtechnoPresets();
    loadHardstylePresets();
}

void PresetManager::loadHardcorePresets()
{
    // Hardcore Gabber Kick
    {
        PresetData preset;
        preset.genre = HARDCORE;
        preset.bpm = 180.0f;
        preset.description = "Aggressive gabber kick with hard clipping and bitcrush";
        
        // Create state for this preset
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Square Wave @ 60Hz
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 1, nullptr); // SQUARE
        osc1State.setProperty("frequency", 60.0f, nullptr);
        osc1State.setProperty("pulseWidth", 0.3f, nullptr);
        osc1State.setProperty("level", 0.8f, nullptr);
        
        // Oscillator 2: Noise
        auto osc2State = state.getOrCreateChildWithName("Oscillator2", nullptr);
        osc2State.setProperty("waveform", 4, nullptr); // NOISE
        osc2State.setProperty("noiseColor", 0, nullptr); // White
        osc2State.setProperty("level", 0.5f, nullptr);
        
        // ADSR for both oscillators
        auto adsrOsc1State = state.getOrCreateChildWithName("ADSR_Osc1", nullptr);
        adsrOsc1State.setProperty("attack", 5.0f, nullptr);
        adsrOsc1State.setProperty("decay", 150.0f, nullptr);
        adsrOsc1State.setProperty("sustain", 0.0f, nullptr);
        adsrOsc1State.setProperty("release", 200.0f, nullptr);
        
        auto adsrOsc2State = state.getOrCreateChildWithName("ADSR_Osc2", nullptr);
        adsrOsc2State.setProperty("attack", 0.0f, nullptr);
        adsrOsc2State.setProperty("decay", 100.0f, nullptr);
        adsrOsc2State.setProperty("sustain", 0.0f, nullptr);
        adsrOsc2State.setProperty("release", 150.0f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 0, nullptr); // LOWPASS
        filterAState.setProperty("cutoff", 300.0f, nullptr);
        filterAState.setProperty("resonance", 0.4f, nullptr);
        filterAState.setProperty("slope", 1, nullptr); // 24dB
        
        // Distortion Unit
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("algorithm", 1, nullptr); // HARD_CLIP
        distortionState.setProperty("drive", 0.8f, nullptr);
        distortionState.setProperty("output", 0.0f, nullptr);
        distortionState.setProperty("mix", 1.0f, nullptr);
        
        // Multiband distortion
        distortionState.setProperty("multibandEnabled", false, nullptr);
        
        preset.state = state;
        presets["Hardcore Gabber Kick"] = preset;
    }
    
    // Hardcore Barking Kick
    {
        PresetData preset;
        preset.genre = HARDCORE;
        preset.bpm = 170.0f;
        preset.description = "Barking kick with aggressive distortion chain";
        
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Sine for sub
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 0, nullptr); // SINE
        osc1State.setProperty("frequency", 50.0f, nullptr);
        osc1State.setProperty("level", 0.7f, nullptr);
        
        // Oscillator 2: Saw for harmonics
        auto osc2State = state.getOrCreateChildWithName("Oscillator2", nullptr);
        osc2State.setProperty("waveform", 2, nullptr); // SAW
        osc2State.setProperty("frequency", 100.0f, nullptr);
        osc2State.setProperty("level", 0.6f, nullptr);
        
        // Sub Generator
        auto subState = state.getOrCreateChildWithName("SubGenerator", nullptr);
        subState.setProperty("frequency", 40.0f, nullptr);
        subState.setProperty("saturation", 0.3f, nullptr);
        subState.setProperty("level", 0.8f, nullptr);
        
        // ADSR settings
        auto adsrOsc1State = state.getOrCreateChildWithName("ADSR_Osc1", nullptr);
        adsrOsc1State.setProperty("attack", 0.0f, nullptr);
        adsrOsc1State.setProperty("decay", 200.0f, nullptr);
        adsrOsc1State.setProperty("sustain", 0.0f, nullptr);
        adsrOsc1State.setProperty("release", 300.0f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 0, nullptr); // LOWPASS
        filterAState.setProperty("cutoff", 200.0f, nullptr);
        filterAState.setProperty("resonance", 0.5f, nullptr);
        
        // Distortion Unit - Multiband
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("multibandEnabled", true, nullptr);
        distortionState.setProperty("lowMidCrossover", 200.0f, nullptr);
        distortionState.setProperty("midHighCrossover", 2000.0f, nullptr);
        distortionState.setProperty("lowAlgorithm", 1, nullptr); // HARD_CLIP
        distortionState.setProperty("midAlgorithm", 2, nullptr); // CHEBYSHEV
        distortionState.setProperty("highAlgorithm", 0, nullptr); // SOFT_CLIP
        distortionState.setProperty("lowDrive", 0.8f, nullptr);
        distortionState.setProperty("midDrive", 0.7f, nullptr);
        distortionState.setProperty("highDrive", 0.5f, nullptr);
        
        preset.state = state;
        presets["Hardcore Barking Kick"] = preset;
    }
}

void PresetManager::loadHardtechnoPresets()
{
    // Hardtechno Knocking Kick
    {
        PresetData preset;
        preset.genre = HARDTECHNO;
        preset.bpm = 150.0f;
        preset.description = "Punchy knocking kick with tape saturation";
        
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Sine for sub
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 0, nullptr); // SINE
        osc1State.setProperty("frequency", 50.0f, nullptr);
        osc1State.setProperty("level", 0.8f, nullptr);
        
        // Oscillator 2: FM synthesis
        auto osc2State = state.getOrCreateChildWithName("Oscillator2", nullptr);
        osc2State.setProperty("waveform", 6, nullptr); // FM
        osc2State.setProperty("fmIndex", 5.0f, nullptr);
        osc2State.setProperty("modulatorFrequency", 150.0f, nullptr);
        osc2State.setProperty("level", 0.5f, nullptr);
        
        // Sampler: 909 Kick
        auto samplerState = state.getOrCreateChildWithName("Sampler", nullptr);
        samplerState.setProperty("pitch", -12.0f, nullptr); // Octave down
        samplerState.setProperty("level", 0.7f, nullptr);
        
        // ADSR settings
        auto adsrOsc1State = state.getOrCreateChildWithName("ADSR_Osc1", nullptr);
        adsrOsc1State.setProperty("attack", 2.0f, nullptr);
        adsrOsc1State.setProperty("decay", 300.0f, nullptr);
        adsrOsc1State.setProperty("sustain", 0.0f, nullptr);
        adsrOsc1State.setProperty("release", 300.0f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 0, nullptr); // LOWPASS
        filterAState.setProperty("cutoff", 200.0f, nullptr);
        filterAState.setProperty("resonance", 0.3f, nullptr);
        filterAState.setProperty("slope", 0, nullptr); // 12dB
        
        // Distortion Unit - Tape
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("algorithm", 4, nullptr); // TAPE
        distortionState.setProperty("drive", 0.5f, nullptr);
        distortionState.setProperty("bias", 0.2f, nullptr);
        distortionState.setProperty("hiss", 0.1f, nullptr);
        
        // Compressor
        auto compressorState = state.getOrCreateChildWithName("Compressor", nullptr);
        compressorState.setProperty("threshold", -12.0f, nullptr);
        compressorState.setProperty("ratio", 4.0f, nullptr);
        compressorState.setProperty("attack", 10.0f, nullptr);
        compressorState.setProperty("release", 100.0f, nullptr);
        
        preset.state = state;
        presets["Hardtechno Knocking Kick"] = preset;
    }
    
    // Hardtechno Deep Kick
    {
        PresetData preset;
        preset.genre = HARDTECHNO;
        preset.bpm = 145.0f;
        preset.description = "Deep punchy kick with multiband saturation";
        
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Sine for fundamental
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 0, nullptr); // SINE
        osc1State.setProperty("frequency", 55.0f, nullptr);
        osc1State.setProperty("level", 0.9f, nullptr);
        
        // Sub Generator
        auto subState = state.getOrCreateChildWithName("SubGenerator", nullptr);
        subState.setProperty("frequency", 30.0f, nullptr);
        subState.setProperty("saturation", 0.4f, nullptr);
        subState.setProperty("level", 0.8f, nullptr);
        
        // Noise Rumble
        auto noiseState = state.getOrCreateChildWithName("NoiseRumble", nullptr);
        noiseState.setProperty("filterType", 2, nullptr); // BANDPASS
        noiseState.setProperty("cutoffFrequency", 100.0f, nullptr);
        noiseState.setProperty("resonance", 0.6f, nullptr);
        noiseState.setProperty("level", 0.4f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 0, nullptr); // LOWPASS
        filterAState.setProperty("cutoff", 100.0f, nullptr);
        filterAState.setProperty("resonance", 0.2f, nullptr);
        
        // Distortion Unit - Multiband
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("multibandEnabled", true, nullptr);
        distortionState.setProperty("lowAlgorithm", 0, nullptr); // SOFT_CLIP
        distortionState.setProperty("midAlgorithm", 4, nullptr); // TAPE
        distortionState.setProperty("highAlgorithm", 2, nullptr); // CHEBYSHEV
        
        preset.state = state;
        presets["Hardtechno Deep Kick"] = preset;
    }
}

void PresetManager::loadHardstylePresets()
{
    // Hardstyle Bonk Kick
    {
        PresetData preset;
        preset.genre = HARDSTYLE;
        preset.bpm = 150.0f;
        preset.description = "Classic hardstyle bonk kick with pitch envelope";
        
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Sine for sub
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 0, nullptr); // SINE
        osc1State.setProperty("frequency", 55.0f, nullptr);
        osc1State.setProperty("level", 0.8f, nullptr);
        
        // Oscillator 2: Wavetable
        auto osc2State = state.getOrCreateChildWithName("Oscillator2", nullptr);
        osc2State.setProperty("waveform", 5, nullptr); // WAVETABLE
        osc2State.setProperty("wavetablePosition", 0.7f, nullptr);
        osc2State.setProperty("frequency", 110.0f, nullptr);
        osc2State.setProperty("level", 0.6f, nullptr);
        
        // Sub Generator
        auto subState = state.getOrCreateChildWithName("SubGenerator", nullptr);
        subState.setProperty("frequency", 40.0f, nullptr);
        subState.setProperty("level", 0.7f, nullptr);
        
        // ADSR with pitch envelope
        auto adsrOsc1State = state.getOrCreateChildWithName("ADSR_Osc1", nullptr);
        adsrOsc1State.setProperty("attack", 0.0f, nullptr);
        adsrOsc1State.setProperty("decay", 500.0f, nullptr);
        adsrOsc1State.setProperty("sustain", 0.2f, nullptr);
        adsrOsc1State.setProperty("release", 800.0f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 2, nullptr); // BANDPASS
        filterAState.setProperty("cutoff", 50.0f, nullptr);
        filterAState.setProperty("resonance", 0.6f, nullptr);
        
        // Distortion Unit - Multiband
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("multibandEnabled", true, nullptr);
        distortionState.setProperty("lowMidCrossover", 100.0f, nullptr);
        distortionState.setProperty("midHighCrossover", 2000.0f, nullptr);
        distortionState.setProperty("lowAlgorithm", 0, nullptr); // SOFT_CLIP
        distortionState.setProperty("midAlgorithm", 2, nullptr); // CHEBYSHEV
        distortionState.setProperty("highAlgorithm", 4, nullptr); // TAPE
        
        // EQ for shaping
        auto eqState = state.getOrCreateChildWithName("ParametricEQ", nullptr);
        auto band1State = eqState.getOrCreateChildWithName("Band0", nullptr);
        band1State.setProperty("type", 2, nullptr); // LOW_SHELF
        band1State.setProperty("frequency", 40.0f, nullptr);
        band1State.setProperty("gain", 2.0f, nullptr);
        
        auto band2State = eqState.getOrCreateChildWithName("Band1", nullptr);
        band2State.setProperty("type", 0, nullptr); // BELL
        band2State.setProperty("frequency", 100.0f, nullptr);
        band2State.setProperty("gain", -3.0f, nullptr);
        
        auto band3State = eqState.getOrCreateChildWithName("Band2", nullptr);
        band3State.setProperty("type", 0, nullptr); // BELL
        band3State.setProperty("frequency", 2000.0f, nullptr);
        band3State.setProperty("gain", 4.0f, nullptr);
        
        preset.state = state;
        presets["Hardstyle Bonk Kick"] = preset;
    }
    
    // Hardstyle Tail Kick
    {
        PresetData preset;
        preset.genre = HARDSTYLE;
        preset.bpm = 140.0f;
        preset.description = "Hardstyle kick with long tail and reverb";
        
        juce::ValueTree state("HardeKickSynth");
        
        // Oscillator 1: Sine
        auto osc1State = state.getOrCreateChildWithName("Oscillator1", nullptr);
        osc1State.setProperty("waveform", 0, nullptr); // SINE
        osc1State.setProperty("frequency", 60.0f, nullptr);
        osc1State.setProperty("level", 0.7f, nullptr);
        
        // Noise Rumble for tail
        auto noiseState = state.getOrCreateChildWithName("NoiseRumble", nullptr);
        noiseState.setProperty("filterType", 2, nullptr); // BANDPASS
        noiseState.setProperty("cutoffFrequency", 200.0f, nullptr);
        noiseState.setProperty("resonance", 0.5f, nullptr);
        noiseState.setProperty("decayTime", 1000.0f, nullptr); // 1 second tail
        noiseState.setProperty("level", 0.5f, nullptr);
        
        // ADSR for noise
        auto adsrNoiseState = state.getOrCreateChildWithName("ADSR_Noise", nullptr);
        adsrNoiseState.setProperty("attack", 0.0f, nullptr);
        adsrNoiseState.setProperty("decay", 800.0f, nullptr);
        adsrNoiseState.setProperty("sustain", 0.0f, nullptr);
        adsrNoiseState.setProperty("release", 1000.0f, nullptr);
        
        // Filter Bank
        auto filterState = state.getOrCreateChildWithName("FilterBank", nullptr);
        auto filterAState = filterState.getOrCreateChildWithName("FilterA", nullptr);
        filterAState.setProperty("type", 0, nullptr); // LOWPASS
        filterAState.setProperty("cutoff", 100.0f, nullptr);
        filterAState.setProperty("resonance", 0.4f, nullptr);
        
        // Distortion Unit
        auto distortionState = state.getOrCreateChildWithName("DistortionUnit", nullptr);
        distortionState.setProperty("algorithm", 0, nullptr); // SOFT_CLIP
        distortionState.setProperty("drive", 0.6f, nullptr);
        
        // Reverb for tail
        auto reverbState = state.getOrCreateChildWithName("Reverb", nullptr);
        reverbState.setProperty("type", 0, nullptr); // HALL
        reverbState.setProperty("decay", 2.0f, nullptr);
        reverbState.setProperty("preDelay", 50.0f, nullptr);
        reverbState.setProperty("size", 0.7f, nullptr);
        reverbState.setProperty("damping", 0.3f, nullptr);
        reverbState.setProperty("mix", 0.4f, nullptr);
        
        preset.state = state;
        presets["Hardstyle Tail Kick"] = preset;
    }
}

PresetManager::PresetData PresetManager::createPresetFromCurrentState()
{
    PresetData preset;
    preset.genre = CUSTOM;
    preset.bpm = 128.0f;
    preset.description = "User preset";
    
    if (audioEngine != nullptr)
    {
        juce::ValueTree state("HardeKickSynth");
        audioEngine->saveState(state);
        preset.state = state;
    }
    
    return preset;
}

void PresetManager::saveState(juce::ValueTree& state)
{
    // Save current preset
    state.setProperty("currentPreset", currentPreset, nullptr);
    
    // Save all presets
    for (const auto& pair : presets)
    {
        auto presetState = state.getOrCreateChildWithName("Preset_" + pair.first, nullptr);
        presetState.setProperty("genre", static_cast<int>(pair.second.genre), nullptr);
        presetState.setProperty("bpm", pair.second.bpm, nullptr);
        presetState.setProperty("description", pair.second.description, nullptr);
        
        // Save the preset state
        auto presetDataState = presetState.getOrCreateChildWithName("Data", nullptr);
        presetDataState.copyPropertiesAndChildrenFrom(pair.second.state, nullptr);
    }
}

void PresetManager::loadState(const juce::ValueTree& state)
{
    // Load current preset
    if (auto cp = state.getProperty("currentPreset"))
        currentPreset = static_cast<juce::String>(*cp);
    
    // Load all presets
    presets.clear();
    
    for (int i = 0; ; ++i)
    {
        auto presetState = state.getChildWithName("Preset_" + juce::String(i));
        if (presetState.get() == nullptr) break;
        
        PresetData preset;
        
        if (auto genre = presetState->getProperty("genre"))
            preset.genre = static_cast<Genre>(static_cast<int>(*genre));
        
        if (auto bpm = presetState->getProperty("bpm"))
            preset.bpm = static_cast<float>(*bpm);
        
        if (auto desc = presetState->getProperty("description"))
            preset.description = static_cast<juce::String>(*desc);
        
        // Load preset data
        if (auto dataState = presetState->getChildWithName("Data"))
        {
            preset.state = *dataState;
        }
        
        presets["Preset_" + juce::String(i)] = preset;
    }
    
    // If no presets loaded, load factory presets
    if (presets.empty())
    {
        loadFactoryPresets();
    }
}
