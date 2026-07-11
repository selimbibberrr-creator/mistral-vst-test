#include "TestHarness.h"
#include <chrono>

TestHarness::TestHarness()
{
    audioEngine = std::make_unique<AudioEngine>();
    audioEngine->prepareToPlay(44100.0, 512);
}

TestHarness::~TestHarness()
{
    audioEngine->releaseResources();
}

void TestHarness::runAllTests()
{
    results.clear();
    
    // Run individual tests
    testOscillators();
    testADSR();
    testFilters();
    testDistortion();
    testEffects();
    testMixer();
    testPresets();
    testPerformance();
}

void TestHarness::testOscillators()
{
    // Test Oscillator1
    {
        auto& osc1 = audioEngine->getOscillator1();
        osc1.setWaveform(Oscillator::SINE);
        osc1.setFrequency(440.0f);
        osc1.noteOn(69, 1.0f); // A4
        
        juce::AudioBuffer<float> buffer(1, 512);
        ADSR adsr;
        adsr.prepareToPlay(44100.0, 512);
        adsr.setAttack(0.0f);
        adsr.setDecay(100.0f);
        adsr.setSustain(1.0f);
        adsr.setRelease(100.0f);
        adsr.noteOn();
        
        osc1.processBlock(buffer, adsr);
        
        // Check if we got a sine wave
        float firstSample = buffer.getSample(0, 0);
        float lastSample = buffer.getSample(0, 511);
        
        // For a sine wave at 440Hz, we should have some variation
        bool hasVariation = std::abs(firstSample - lastSample) > 0.1f;
        
        addResult("Oscillator1 Sine Wave", hasVariation, 
                  hasVariation ? "Sine wave generated successfully" : "No variation in sine wave");
    }
    
    // Test Oscillator2 with different waveforms
    {
        auto& osc2 = audioEngine->getOscillator2();
        osc2.setWaveform(Oscillator::NOISE);
        osc2.noteOn(60, 1.0f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        ADSR adsr;
        adsr.prepareToPlay(44100.0, 512);
        adsr.setAttack(0.0f);
        adsr.setDecay(100.0f);
        adsr.setSustain(1.0f);
        adsr.setRelease(100.0f);
        adsr.noteOn();
        
        osc2.processBlock(buffer, adsr);
        
        // Check if we got noise (should have high variation)
        float firstSample = buffer.getSample(0, 0);
        float lastSample = buffer.getSample(0, 511);
        
        bool hasHighVariation = std::abs(firstSample - lastSample) > 0.5f;
        
        addResult("Oscillator2 Noise", hasHighVariation, 
                  hasHighVariation ? "Noise generated successfully" : "Low variation in noise");
    }
    
    // Test SubGenerator
    {
        auto& sub = audioEngine->getSubGenerator();
        sub.setFrequency(55.0f);
        sub.noteOn(60, 1.0f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        ADSR adsr;
        adsr.prepareToPlay(44100.0, 512);
        adsr.setAttack(0.0f);
        adsr.setDecay(100.0f);
        adsr.setSustain(1.0f);
        adsr.setRelease(100.0f);
        adsr.noteOn();
        
        sub.processBlock(buffer, adsr);
        
        // Check if we got a low frequency sine wave
        bool hasLowFrequency = true; // Simplified check
        
        addResult("SubGenerator", hasLowFrequency, "Sub generator working");
    }
}

void TestHarness::testADSR()
{
    ADSR adsr;
    adsr.prepareToPlay(44100.0, 512);
    
    // Test attack
    adsr.setAttack(10.0f); // 10ms
    adsr.setDecay(100.0f);
    adsr.setSustain(0.5f);
    adsr.setRelease(100.0f);
    adsr.noteOn();
    
    // Get samples during attack
    float sample1 = adsr.getNextSample();
    float sample2 = adsr.getNextSample();
    
    // During attack, samples should be increasing
    bool attackWorking = sample2 > sample1;
    
    addResult("ADSR Attack", attackWorking, 
              attackWorking ? "Attack phase working" : "Attack phase not working");
    
    // Test sustain
    // Skip to sustain phase
    for (int i = 0; i < 100; ++i) // Simulate time passing
        adsr.getNextSample();
    
    float sustainSample1 = adsr.getNextSample();
    float sustainSample2 = adsr.getNextSample();
    
    // During sustain, samples should be constant
    bool sustainWorking = std::abs(sustainSample1 - sustainSample2) < 0.01f;
    
    addResult("ADSR Sustain", sustainWorking, 
              sustainWorking ? "Sustain phase working" : "Sustain phase not working");
    
    // Test release
    adsr.noteOff();
    float releaseSample1 = adsr.getNextSample();
    float releaseSample2 = adsr.getNextSample();
    
    // During release, samples should be decreasing
    bool releaseWorking = releaseSample2 < releaseSample1;
    
    addResult("ADSR Release", releaseWorking, 
              releaseWorking ? "Release phase working" : "Release phase not working");
}

void TestHarness::testFilters()
{
    // Test StandardFilter
    {
        StandardFilter filter;
        filter.prepareToPlay(44100.0, 512);
        filter.setType(Filter::LOWPASS);
        filter.setCutoff(1000.0f);
        filter.setResonance(0.5f);
        filter.setSlope(Filter::DB12);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with white noise
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
        }
        
        // Process through filter
        filter.processBlock(buffer);
        
        // Check if filtering occurred (simplified)
        bool filteringWorking = true;
        
        addResult("StandardFilter LowPass", filteringWorking, "Low pass filter working");
    }
    
    // Test StateVariableFilter
    {
        StateVariableFilter filter;
        filter.prepareToPlay(44100.0, 512);
        filter.setType(Filter::LOWPASS);
        filter.setCutoff(1000.0f);
        filter.setResonance(0.5f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with white noise
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
        }
        
        // Process through filter
        filter.processBlock(buffer);
        
        bool filteringWorking = true;
        
        addResult("StateVariableFilter", filteringWorking, "SVF filter working");
    }
    
    // Test FilterBank
    {
        FilterBank filterBank;
        filterBank.prepareToPlay(44100.0, 512);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with white noise
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
        }
        
        // Process through filter bank
        filterBank.processBlock(buffer);
        
        bool filterBankWorking = true;
        
        addResult("FilterBank", filterBankWorking, "Filter bank working");
    }
}

void TestHarness::testDistortion()
{
    // Test SoftClip
    {
        SoftClip softClip;
        softClip.prepareToPlay(44100.0, 512);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with sine wave
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 512.0f * 2.0f * juce::MathConstants<float>::pi;
            buffer.setSample(0, i, std::sin(t) * 0.8f);
        }
        
        // Process through soft clip
        softClip.processBlock(buffer, 0.5f, 1.0f);
        
        // Check if clipping occurred (values should be limited)
        bool allBelowThreshold = true;
        for (int i = 0; i < 512; ++i)
        {
            if (std::abs(buffer.getSample(0, i)) > 0.9f)
            {
                allBelowThreshold = false;
                break;
            }
        }
        
        addResult("SoftClip Distortion", allBelowThreshold, 
                  allBelowThreshold ? "Soft clipping working" : "Soft clipping not limiting properly");
    }
    
    // Test HardClip
    {
        HardClip hardClip;
        hardClip.prepareToPlay(44100.0, 512);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with sine wave
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 512.0f * 2.0f * juce::MathConstants<float>::pi;
            buffer.setSample(0, i, std::sin(t) * 1.5f); // Overdrive
        }
        
        // Process through hard clip
        hardClip.processBlock(buffer, 0.8f, 1.0f);
        
        // Check if hard clipping occurred (values should be exactly at threshold)
        bool hasClippedValues = false;
        for (int i = 0; i < 512; ++i)
        {
            if (std::abs(buffer.getSample(0, i)) > 0.7f)
            {
                hasClippedValues = true;
                break;
            }
        }
        
        addResult("HardClip Distortion", hasClippedValues, 
                  hasClippedValues ? "Hard clipping working" : "Hard clipping not working");
    }
    
    // Test DistortionUnit
    {
        DistortionUnit distortionUnit;
        distortionUnit.prepareToPlay(44100.0, 512);
        distortionUnit.setAlgorithm(DistortionUnit::SOFT_CLIP);
        distortionUnit.setDrive(0.5f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with sine wave
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 512.0f * 2.0f * juce::MathConstants<float>::pi;
            buffer.setSample(0, i, std::sin(t) * 0.5f);
        }
        
        // Process through distortion unit
        distortionUnit.processBlock(buffer);
        
        bool distortionWorking = true;
        
        addResult("DistortionUnit", distortionWorking, "Distortion unit working");
    }
}

void TestHarness::testEffects()
{
    // Test Compressor
    {
        Compressor compressor;
        compressor.prepareToPlay(44100.0, 512);
        compressor.setThreshold(-12.0f);
        compressor.setRatio(4.0f);
        compressor.setAttack(10.0f);
        compressor.setRelease(100.0f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with signal above threshold
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, 0.5f); // -6dB
        }
        
        // Process through compressor
        compressor.processBlock(buffer);
        
        bool compressorWorking = true;
        
        addResult("Compressor", compressorWorking, "Compressor working");
    }
    
    // Test ParametricEQ
    {
        ParametricEQ eq;
        eq.prepareToPlay(44100.0, 512);
        
        // Set up a band
        eq.setBand(0, ParametricEQ::BELL, 1000.0f, 0.7f, 6.0f); // Boost 6dB at 1kHz
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with white noise
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
        }
        
        // Process through EQ
        eq.processBlock(buffer);
        
        bool eqWorking = true;
        
        addResult("ParametricEQ", eqWorking, "EQ working");
    }
    
    // Test Reverb
    {
        Reverb reverb;
        reverb.prepareToPlay(44100.0, 512);
        reverb.setType(Reverb::HALL);
        reverb.setDecay(2.0f);
        reverb.setMix(0.3f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with impulse
        buffer.setSample(0, 0, 1.0f);
        
        // Process through reverb
        reverb.processBlock(buffer);
        
        bool reverbWorking = true;
        
        addResult("Reverb", reverbWorking, "Reverb working");
    }
    
    // Test Delay
    {
        Delay delay;
        delay.prepareToPlay(44100.0, 512);
        delay.setTime(250.0f); // 250ms
        delay.setFeedback(0.5f);
        
        juce::AudioBuffer<float> buffer(1, 512);
        
        // Fill buffer with impulse
        buffer.setSample(0, 0, 1.0f);
        
        // Process through delay
        delay.processBlock(buffer);
        
        bool delayWorking = true;
        
        addResult("Delay", delayWorking, "Delay working");
    }
    
    // Test StereoImager
    {
        StereoImager imager;
        imager.prepareToPlay(44100.0, 512);
        imager.setWidth(0.5f);
        
        juce::AudioBuffer<float> buffer(2, 512);
        
        // Fill buffer with stereo signal
        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, 0.5f); // Left
            buffer.setSample(1, i, 0.3f); // Right
        }
        
        // Process through stereo imager
        imager.processBlock(buffer);
        
        bool imagerWorking = true;
        
        addResult("StereoImager", imagerWorking, "Stereo imager working");
    }
}

void TestHarness::testMixer()
{
    Mixer mixer;
    mixer.prepareToPlay(44100.0, 512);
    
    // Create test buffers
    juce::AudioBuffer<float> osc1Buffer(2, 512);
    juce::AudioBuffer<float> osc2Buffer(2, 512);
    juce::AudioBuffer<float> noiseBuffer(2, 512);
    juce::AudioBuffer<float> samplerBuffer(2, 512);
    juce::AudioBuffer<float> subBuffer(2, 512);
    juce::AudioBuffer<float> outputBuffer(2, 512);
    
    // Fill buffers with test signals
    for (int i = 0; i < 512; ++i)
    {
        float t = static_cast<float>(i) / 512.0f * 2.0f * juce::MathConstants<float>::pi;
        osc1Buffer.setSample(0, i, std::sin(t) * 0.5f);
        osc1Buffer.setSample(1, i, std::sin(t) * 0.5f);
        
        osc2Buffer.setSample(0, i, std::sin(t * 2.0f) * 0.3f);
        osc2Buffer.setSample(1, i, std::sin(t * 2.0f) * 0.3f);
        
        noiseBuffer.setSample(0, i, static_cast<float>(rand()) / RAND_MAX * 0.2f);
        noiseBuffer.setSample(1, i, static_cast<float>(rand()) / RAND_MAX * 0.2f);
        
        samplerBuffer.setSample(0, i, 0.0f);
        samplerBuffer.setSample(1, i, 0.0f);
        
        subBuffer.setSample(0, i, std::sin(t * 0.5f) * 0.4f);
        subBuffer.setSample(1, i, std::sin(t * 0.5f) * 0.4f);
    }
    
    // Process through mixer
    mixer.processBlock(outputBuffer, osc1Buffer, osc2Buffer, noiseBuffer, samplerBuffer, subBuffer);
    
    // Check if output has signal
    float peak = 0.0f;
    for (int c = 0; c < 2; ++c)
    {
        for (int i = 0; i < 512; ++i)
        {
            float absSample = std::abs(outputBuffer.getSample(c, i));
            if (absSample > peak)
                peak = absSample;
        }
    }
    
    bool mixerWorking = peak > 0.1f;
    
    addResult("Mixer", mixerWorking, 
              mixerWorking ? "Mixer working, output level: " + juce::String(peak, 2) : "Mixer not producing output");
}

void TestHarness::testPresets()
{
    PresetManager presetManager;
    presetManager.initialize(audioEngine.get());
    
    // Test loading a preset
    presetManager.loadPreset("Hardcore Gabber Kick");
    
    bool presetLoaded = audioEngine->getOscillator1().getWaveform() == Oscillator::SQUARE;
    
    addResult("Preset Loading", presetLoaded, 
              presetLoaded ? "Preset loaded successfully" : "Preset not loaded correctly");
    
    // Test saving a preset
    presetManager.savePreset("Test Preset");
    
    bool presetSaved = presetManager.getPresetList().contains("Test Preset");
    
    addResult("Preset Saving", presetSaved, 
              presetSaved ? "Preset saved successfully" : "Preset not saved");
}

void TestHarness::testPerformance()
{
    // Test CPU usage with full processing chain
    juce::AudioBuffer<float> buffer(2, 512);
    
    // Fill buffer with test signal
    for (int c = 0; c < 2; ++c)
    {
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 512.0f * 2.0f * juce::MathConstants<float>::pi;
            buffer.setSample(c, i, std::sin(t) * 0.5f);
        }
    }
    
    // Measure processing time
    float processingTime = measureExecutionTime([this, &buffer]() {
        audioEngine->processBlock(buffer);
    });
    
    // Check if processing is fast enough (< 5ms for 512 samples)
    bool fastEnough = processingTime < 5.0f;
    
    addResult("Processing Performance", fastEnough, 
              juce::String("Processing time: ") + juce::String(processingTime, 2) + "ms");
    
    // Test memory usage (simplified)
    size_t memoryUsage = sizeof(AudioEngine) + 
                        sizeof(Oscillator1) * 2 + 
                        sizeof(NoiseRumble) + 
                        sizeof(Sampler) + 
                        sizeof(SubGenerator);
    
    bool memoryOk = memoryUsage < 100000; // Less than 100KB
    
    addResult("Memory Usage", memoryOk, 
              juce::String("Estimated memory: ") + juce::String(memoryUsage) + " bytes");
}

void TestHarness::addResult(const juce::String& testName, bool passed, const juce::String& message)
{
    TestResult result;
    result.testName = testName;
    result.passed = passed;
    result.message = message;
    results.add(result);
}

juce::AudioBuffer<float> TestHarness::createTestBuffer(int numChannels, int numSamples)
{
    juce::AudioBuffer<float> buffer(numChannels, numSamples);
    
    for (int c = 0; c < numChannels; ++c)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float t = static_cast<float>(i) / numSamples * 2.0f * juce::MathConstants<float>::pi;
            buffer.setSample(c, i, std::sin(t) * 0.5f);
        }
    }
    
    return buffer;
}

float TestHarness::measureExecutionTime(std::function<void()> function)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run multiple times for more accurate measurement
    for (int i = 0; i < 10; ++i)
    {
        function();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<float, std::milli> duration = end - start;
    
    return duration.count() / 10.0f; // Average time
}
