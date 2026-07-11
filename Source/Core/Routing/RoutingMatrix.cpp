#include "RoutingMatrix.h"

RoutingMatrix::RoutingMatrix()
{
    // Initialize connection matrix
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        for (int j = 0; j < NUM_MODULES; ++j)
        {
            connectionMatrix[i][j] = 0.0f;
        }
    }
    
    initializeDefaultRouting();
}

RoutingMatrix::~RoutingMatrix()
{
}

void RoutingMatrix::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    
    // Initialize buffers
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        moduleBuffers[i].setSize(2, blockSize);
    }
}

void RoutingMatrix::releaseResources()
{
}

void RoutingMatrix::connect(ModuleIndex source, ModuleIndex destination, float gain)
{
    if (source >= 0 && source < NUM_MODULES && destination >= 0 && destination < NUM_MODULES)
    {
        connectionMatrix[source][destination] = juce::jlimit(0.0f, 1.0f, gain);
    }
}

void RoutingMatrix::disconnect(ModuleIndex source, ModuleIndex destination)
{
    if (source >= 0 && source < NUM_MODULES && destination >= 0 && destination < NUM_MODULES)
    {
        connectionMatrix[source][destination] = 0.0f;
    }
}

void RoutingMatrix::disconnectAll(ModuleIndex module)
{
    if (module >= 0 && module < NUM_MODULES)
    {
        for (int i = 0; i < NUM_MODULES; ++i)
        {
            connectionMatrix[module][i] = 0.0f;
            connectionMatrix[i][module] = 0.0f;
        }
    }
}

void RoutingMatrix::setGain(ModuleIndex source, ModuleIndex destination, float gain)
{
    if (source >= 0 && source < NUM_MODULES && destination >= 0 && destination < NUM_MODULES)
    {
        connectionMatrix[source][destination] = juce::jlimit(0.0f, 1.0f, gain);
    }
}

float RoutingMatrix::getGain(ModuleIndex source, ModuleIndex destination) const
{
    if (source >= 0 && source < NUM_MODULES && destination >= 0 && destination < NUM_MODULES)
    {
        return connectionMatrix[source][destination];
    }
    return 0.0f;
}

bool RoutingMatrix::isConnected(ModuleIndex source, ModuleIndex destination) const
{
    if (source >= 0 && source < NUM_MODULES && destination >= 0 && destination < NUM_MODULES)
    {
        return connectionMatrix[source][destination] > 0.0f;
    }
    return false;
}

void RoutingMatrix::processBlock(juce::AudioBuffer<float>& buffer)
{
    // In a real implementation, this would process the signal through the routing matrix
    // For now, we'll just pass the signal through (the actual routing is handled in AudioEngine)
    
    // This is a placeholder - the real routing logic would be more complex
    // and would involve processing each module in the correct order
}

void RoutingMatrix::initializeDefaultRouting()
{
    // Set up default signal flow:
    // OSC1, OSC2, NOISE, SAMPLER, SUB -> MIXER -> FILTER_A -> FILTER_B -> DISTORTION -> COMPRESSOR -> EQ -> REVERB -> DELAY -> STEREO_IMAGER -> MASTER_OUT
    
    // Connect sources to mixer
    connect(OSC1, MIXER, 1.0f);
    connect(OSC2, MIXER, 1.0f);
    connect(NOISE, MIXER, 1.0f);
    connect(SAMPLER, MIXER, 1.0f);
    connect(SUB, MIXER, 1.0f);
    
    // Connect mixer to filter A
    connect(MIXER, FILTER_A, 1.0f);
    
    // Connect filter A to filter B
    connect(FILTER_A, FILTER_B, 1.0f);
    
    // Connect filter B to distortion
    connect(FILTER_B, DISTORTION, 1.0f);
    
    // Connect distortion to compressor
    connect(DISTORTION, COMPRESSOR, 1.0f);
    
    // Connect compressor to EQ
    connect(COMPRESSOR, EQ, 1.0f);
    
    // Connect EQ to reverb
    connect(EQ, REVERB, 1.0f);
    
    // Connect reverb to delay
    connect(REVERB, DELAY, 1.0f);
    
    // Connect delay to stereo imager
    connect(DELAY, STEREO_IMAGER, 1.0f);
    
    // Connect stereo imager to master out
    connect(STEREO_IMAGER, MASTER_OUT, 1.0f);
}

void RoutingMatrix::saveState(juce::ValueTree& state)
{
    // Save connection matrix
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        for (int j = 0; j < NUM_MODULES; ++j)
        {
            if (connectionMatrix[i][j] > 0.0f)
            {
                auto connectionState = state.getOrCreateChildWithName("Connection" + juce::String(i) + "_" + juce::String(j), nullptr);
                connectionState.setProperty("source", i, nullptr);
                connectionState.setProperty("destination", j, nullptr);
                connectionState.setProperty("gain", connectionMatrix[i][j], nullptr);
            }
        }
    }
}

void RoutingMatrix::loadState(const juce::ValueTree& state)
{
    // Clear all connections
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        for (int j = 0; j < NUM_MODULES; ++j)
        {
            connectionMatrix[i][j] = 0.0f;
        }
    }
    
    // Load connections
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        for (int j = 0; j < NUM_MODULES; ++j)
        {
            auto connectionState = state.getChildWithName("Connection" + juce::String(i) + "_" + juce::String(j));
            if (connectionState.get() != nullptr)
            {
                if (auto gain = connectionState->getProperty("gain"))
                {
                    connectionMatrix[i][j] = static_cast<float>(*gain);
                }
            }
        }
    }
    
    // If no connections loaded, use default routing
    bool hasConnections = false;
    for (int i = 0; i < NUM_MODULES; ++i)
    {
        for (int j = 0; j < NUM_MODULES; ++j)
        {
            if (connectionMatrix[i][j] > 0.0f)
            {
                hasConnections = true;
                break;
            }
        }
        if (hasConnections) break;
    }
    
    if (!hasConnections)
    {
        initializeDefaultRouting();
    }
}
