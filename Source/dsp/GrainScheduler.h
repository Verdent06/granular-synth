#pragma once

#include "Grain.h"

#include "../util/MemoryPool.h"

#include <algorithm>
#include <cmath>
#include <random>

struct GranularParams
{
    float grainSizeMs { 50.0f };
    float density { 20.0f };
    float overlap { 0.5f };
    float pitchSemitones { 0.0f };
    float pitchScatter { 0.0f };
};

class GrainScheduler
{
public:
    static constexpr std::size_t maxGrainsPerVoice = 64;

    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        grainPool = MemoryPool<Grain, maxGrainsPerVoice> {};
        reset();
    }

    void reset() noexcept
    {
        spawnAccumulator = 0.0;
        grainPool = MemoryPool<Grain, maxGrainsPerVoice> {};
    }

    void setParams(const GranularParams& p) noexcept
    {
        params = p;
    }

    void scheduleGrains(double blockDurationSec,
                        const float* sampleData,
                        std::size_t sampleLength,
                        float velocity,
                        std::mt19937& rng) noexcept
    {
        if (sampleData == nullptr || sampleLength < 2)
            return;

        const double grainsPerSecond = static_cast<double>(std::max(0.1f, params.density));
        const double overlapFactor = static_cast<double>(std::clamp(params.overlap, 0.0f, 0.99f));
        const double effectiveRate = grainsPerSecond * (1.0 - overlapFactor * 0.5);
        spawnAccumulator += blockDurationSec * effectiveRate;

        while (spawnAccumulator >= 1.0)
        {
            spawnAccumulator -= 1.0;
            spawnGrain(sampleData, sampleLength, velocity, rng);
        }
    }

    void spawnInitialGrain(const float* sampleData,
                           std::size_t sampleLength,
                           float velocity,
                           std::mt19937& rng) noexcept
    {
        spawnGrain(sampleData, sampleLength, velocity, rng);
    }

    MemoryPool<Grain, maxGrainsPerVoice>& getPool() noexcept
    {
        return grainPool;
    }

private:
    void spawnGrain(const float* /*sampleData*/,
                    std::size_t sampleLength,
                    float velocity,
                    std::mt19937& rng) noexcept
    {
        auto* grain = grainPool.acquire();
        if (grain == nullptr)
            return;

        const std::size_t lengthSamples = static_cast<std::size_t>(
            sampleRate * static_cast<double>(std::clamp(params.grainSizeMs, 5.0f, 500.0f)) * 0.001);

        std::uniform_int_distribution<std::size_t> positionDist(0, sampleLength - lengthSamples - 1);
        std::uniform_real_distribution<float> scatterDist(-params.pitchScatter, params.pitchScatter);

        const float scatter = scatterDist(rng);
        const float semitones = params.pitchSemitones + scatter;

        grain->startSample = static_cast<double>(positionDist(rng));
        grain->readPosition = grain->startSample;
        grain->lengthSamples = std::min(lengthSamples, sampleLength);
        grain->age = 0;
        grain->amplitude = velocity;
        grain->playbackRate = std::pow(2.0, static_cast<double>(semitones) / 12.0);
        grain->active = true;
    }

    GranularParams params {};
    MemoryPool<Grain, maxGrainsPerVoice> grainPool {};
    double sampleRate { 44100.0 };
    double spawnAccumulator { 0.0 };
};
