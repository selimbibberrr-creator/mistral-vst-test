#include "PluginProcessor.h"
#include "PluginEditor.h"

HardeKickSynthAudioProcessor::HardeKickSynthAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    audioEngine = std::make_unique<AudioEngine>();
}

HardeKickSynthAudioProcessor::~HardeKickSynthAudioProcessor()
{
}

void HardeKickSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    audioEngine->prepareToPlay(sampleRate, samplesPerBlock);
}

void HardeKickSynthAudioProcessor::releaseResources()
{
    audioEngine->releaseResources();
}

void HardeKickSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Process MIDI messages
    for (const auto midiMessage : midiMessages)
    {
        auto midiEvent = midiMessage.getMessage();
        audioEngine->processMidiMessage(midiEvent);
    }
    
    // Process audio
    audioEngine->processBlock(buffer);
}

juce::AudioProcessorEditor* HardeKickSynthAudioProcessor::createEditor()
{
    return new HardeKickSynthAudioProcessorEditor(*this);
}

bool HardeKickSynthAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String HardeKickSynthAudioProcessor::getName() const
{
    return "Harde Kick Synth";
}

bool HardeKickSynthAudioProcessor::acceptsMidi() const
{
    return true;
}

bool HardeKickSynthAudioProcessor::producesMidi() const
{
    return false;
}

double HardeKickSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HardeKickSynthAudioProcessor::getNumPrograms()
{
    return 1;
}

int HardeKickSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HardeKickSynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String HardeKickSynthAudioProcessor::getProgramName(int index)
{
    return "Default";
}

void HardeKickSynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void HardeKickSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    auto state = juce::ValueTree("HardeKickSynth");
    audioEngine->saveState(state);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HardeKickSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Load plugin state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xmlState);
        audioEngine->loadState(state);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HardeKickSynthAudioProcessor();
}
