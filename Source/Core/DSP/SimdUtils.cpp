#include "SimdUtils.h"
#include <cpuid.h>

bool SimdUtils::isSimdAvailable()
{
    return checkSSESupport() || checkAVXSupport();
}

bool SimdUtils::checkSSESupport()
{
    #ifdef __SSE__
        return true;
    #else
        unsigned int info[4];
        __cpuid(1, info[0], info[1], info[2], info[3]);
        return (info[3] & (1 << 25)) != 0; // SSE bit
    #endif
}

bool SimdUtils::checkAVXSupport()
{
    #ifdef __AVX__
        return true;
    #else
        unsigned int info[4];
        __cpuid(1, info[0], info[1], info[2], info[3]);
        return (info[2] & (1 << 28)) != 0; // AVX bit
    #endif
}

void SimdUtils::process4SamplesSIMD(float* output, const float* input, 
                                    const float* coefficients, int numSamples)
{
    // This is a placeholder for SIMD-optimized processing
    // In a real implementation, this would use SSE instructions
    
    for (int i = 0; i < numSamples; i += 4)
    {
        if (i + 4 <= numSamples)
        {
            // Load 4 samples
            __m128 inVec = Simd::load4(&input[i]);
            __m128 coeffVec = Simd::load4(coefficients);
            
            // Process (example: multiply by coefficients)
            __m128 outVec = Simd::mul(inVec, coeffVec);
            
            // Store result
            Simd::store4(&output[i], outVec);
        }
        else
        {
            // Process remaining samples without SIMD
            for (int j = i; j < numSamples; ++j)
            {
                output[j] = input[j] * coefficients[0];
            }
        }
    }
}

void SimdUtils::process8SamplesAVX(float* output, const float* input, 
                                  const float* coefficients, int numSamples)
{
    #ifdef __AVX__
        for (int i = 0; i < numSamples; i += 8)
        {
            if (i + 8 <= numSamples)
            {
                // Load 8 samples
                __m256 inVec = _mm256_loadu_ps(&input[i]);
                __m256 coeffVec = _mm256_loadu_ps(coefficients);
                
                // Process
                __m256 outVec = _mm256_mul_ps(inVec, coeffVec);
                
                // Store result
                _mm256_storeu_ps(&output[i], outVec);
            }
            else
            {
                // Process remaining samples without AVX
                for (int j = i; j < numSamples; ++j)
                {
                    output[j] = input[j] * coefficients[0];
                }
            }
        }
    #else
        // Fallback to non-AVX
        process4SamplesSIMD(output, input, coefficients, numSamples);
    #endif
}

void SimdUtils::applyGainSIMD(float* buffer, float gain, int numSamples)
{
    __m128 gainVec = Simd::set1(gain);
    
    for (int i = 0; i < numSamples; i += 4)
    {
        if (i + 4 <= numSamples)
        {
            __m128 vec = Simd::load4(&buffer[i]);
            vec = Simd::mul(vec, gainVec);
            Simd::store4(&buffer[i], vec);
        }
        else
        {
            for (int j = i; j < numSamples; ++j)
            {
                buffer[j] *= gain;
            }
        }
    }
}

void SimdUtils::mixBuffersSIMD(float* output, const float* input1, const float* input2, 
                               float gain1, float gain2, int numSamples)
{
    __m128 gain1Vec = Simd::set1(gain1);
    __m128 gain2Vec = Simd::set1(gain2);
    
    for (int i = 0; i < numSamples; i += 4)
    {
        if (i + 4 <= numSamples)
        {
            __m128 in1 = Simd::load4(&input1[i]);
            __m128 in2 = Simd::load4(&input2[i]);
            
            in1 = Simd::mul(in1, gain1Vec);
            in2 = Simd::mul(in2, gain2Vec);
            
            __m128 out = Simd::add(in1, in2);
            Simd::store4(&output[i], out);
        }
        else
        {
            for (int j = i; j < numSamples; ++j)
            {
                output[j] = input1[j] * gain1 + input2[j] * gain2;
            }
        }
    }
}

void SimdUtils::processFilterSIMD(float* output, const float* input, 
                                 const float* coefficients, float* state, int numSamples)
{
    // This is a placeholder for SIMD-optimized filter processing
    // In a real implementation, this would process 4 samples at once
    
    // For now, use scalar processing
    for (int i = 0; i < numSamples; ++i)
    {
        // Simple direct form II transposed filter
        output[i] = input[i] * coefficients[0] + state[0];
        state[0] = input[i] * coefficients[1] - coefficients[2] * output[i] + state[1];
        state[1] = input[i] * coefficients[3] - coefficients[4] * output[i];
    }
}
