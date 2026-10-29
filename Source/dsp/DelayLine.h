#pragma once

#include "RingBuffer.h"

#include <algorithm>
#include <cmath>

class DelayLine
{
public:
    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        ringBuffer.prepare(17); // 131072 samples (~3s at 44.1kHz)
        reset();
    }

    void reset() noexcept
    {
        ringBuffer.reset();
        feedbackSample = 0.0f;
    }

    void setDelayTimeMs(float ms) noexcept
    {
        targetDelaySamples = static_cast<std::size_t>(sampleRate * static_cast<double>(ms) * 0.001);
        const auto maxDelay = ringBuffer.size() > 1 ? ringBuffer.size() - 1 : 0;
        targetDelaySamples = std::min(targetDelaySamples, maxDelay);
    }

    void setFeedback(float amount) noexcept
    {
        feedback = std::clamp(amount, 0.0f, 0.95f);
    }

    void setMix(float amount) noexcept
    {
        mix = std::clamp(amount, 0.0f, 1.0f);
    }

    float processSample(float input) noexcept
    {
        const float delayed = ringBuffer.readDelay(currentDelaySamples);
        const float wet = input + delayed * feedback;
        ringBuffer.write(wet);

        // One-pole smoothing on delay time to avoid zipper noise.
        if (currentDelaySamples != targetDelaySamples)
        {
            if (currentDelaySamples < targetDelaySamples)
                ++currentDelaySamples;
            else
                --currentDelaySamples;
        }

        feedbackSample = delayed;
        return input * (1.0f - mix) + delayed * mix;
    }

private:
    RingBuffer ringBuffer;
    double sampleRate { 44100.0 };
    std::size_t targetDelaySamples { 2205 };
    std::size_t currentDelaySamples { 2205 };
    float feedback { 0.35f };
    float mix { 0.25f };
    float feedbackSample { 0.0f };
};
