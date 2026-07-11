#pragma once

#include <JuceHeader.h>
#include <immintrin.h> // For SIMD intrinsics

// SIMD utility functions for DSP optimization
class SimdUtils
{
public:
    // Check if SIMD is available
    static bool isSimdAvailable();

    // Process 4 samples at once using SIMD (SSE)
    static void process4SamplesSIMD(float* output, const float* input, 
                                    const float* coefficients, int numSamples);

    // Process 8 samples at once using AVX
    static void process8SamplesAVX(float* output, const float* input, 
                                  const float* coefficients, int numSamples);

    // Apply gain to 4 samples using SIMD
    static void applyGainSIMD(float* buffer, float gain, int numSamples);

    // Mix two buffers using SIMD
    static void mixBuffersSIMD(float* output, const float* input1, const float* input2, 
                               float gain1, float gain2, int numSamples);

    // Filter processing using SIMD (4-pole)
    static void processFilterSIMD(float* output, const float* input, 
                                 const float* coefficients, float* state, int numSamples);

private:
    // Helper functions
    static bool checkSSESupport();
    static bool checkAVXSupport();
};

// Inline SIMD functions for direct use
namespace Simd
{
    // Load 4 floats into SIMD register
    inline __m128 load4(const float* ptr)
    {
        return _mm_loadu_ps(ptr);
    }

    // Store 4 floats from SIMD register
    inline void store4(float* ptr, __m128 vec)
    {
        _mm_storeu_ps(ptr, vec);
    }

    // Add two SIMD vectors
    inline __m128 add(__m128 a, __m128 b)
    {
        return _mm_add_ps(a, b);
    }

    // Subtract two SIMD vectors
    inline __m128 sub(__m128 a, __m128 b)
    {
        return _mm_sub_ps(a, b);
    }

    // Multiply two SIMD vectors
    inline __m128 mul(__m128 a, __m128 b)
    {
        return _mm_mul_ps(a, b);
    }

    // Multiply and add (FMA)
    inline __m128 fma(__m128 a, __m128 b, __m128 c)
    {
        return _mm_add_ps(_mm_mul_ps(a, b), c);
    }

    // Set all elements to same value
    inline __m128 set1(float value)
    {
        return _mm_set1_ps(value);
    }

    // Set to zero
    inline __m128 zero()
    {
        return _mm_setzero_ps();
    }

    // Horizontal add (sum all elements)
    inline float horizontalAdd(__m128 vec)
    {
        __m128 shuf = _mm_movehdup_ps(vec);
        __m128 sums = _mm_add_ps(vec, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        return _mm_cvtss_f32(sums);
    }
}
