#pragma once

#include "GaussianWindow.h"
#include "GrainScheduler.h"
#include "SineOscillator.h"

#include "../util/SampleBuffer.h"

#include <memory>
#include <random>

class GrainEngine
{
public:
    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        scheduler.prepare(sampleRate);
        window.prepare(GaussianWindow::maxGrainSamples);
        rng.seed(0x67526169u);
        useGranular = false;
        sine.prepare(sampleRate);
    }

    void reset() noexcept
    {
        scheduler.reset();
        sine.reset();
    }

    void setParams(const GranularParams& p) noexcept
    {
        params = p;
        scheduler.setParams(params);
    }

    void setUseGranular(bool enabled) noexcept
    {
        useGranular = enabled;
    }

    void noteOn(int noteNumber, float velocity, const SampleBuffer& sampleBuffer) noexcept
    {
        currentNote = noteNumber;
        currentVelocity = velocity;
        sine.setFrequency(SineOscillator::midiNoteToFrequency(noteNumber));
        sine.reset();

        if (useGranular && sampleBuffer.hasActiveSample())
        {
            const auto data = sampleBuffer.getActive();
            scheduler.spawnInitialGrain(data->mono.data(), data->mono.size(), velocity, rng);
        }
    }

    void noteOff() noexcept
    {
    }

    float processSample(const SampleBuffer& sampleBuffer) noexcept
    {
        if (! useGranular || ! sampleBuffer.hasActiveSample())
            return sine.processSample() * 0.25f;

        const auto data = sampleBuffer.getActive();
        const float* sampleData = data->mono.data();
        const std::size_t sampleLength = data->mono.size();

        float output = 0.0f;

        scheduler.getPool().forEachActive([&](Grain& grain) {
            if (! grain.active)
                return;

            const std::size_t idx = static_cast<std::size_t>(grain.readPosition);
            if (idx + 1 >= sampleLength)
            {
                scheduler.getPool().release(&grain);
                return;
            }

            const float frac = static_cast<float>(grain.readPosition - static_cast<double>(idx));
            const float s0 = sampleData[idx];
            const float s1 = sampleData[idx + 1];
            const float sample = s0 + frac * (s1 - s0);
            const float windowValue = window.lookup(grain.age, grain.lengthSamples);

            output += sample * windowValue * grain.amplitude;

            grain.readPosition += grain.playbackRate;
            ++grain.age;

            if (grain.age >= grain.lengthSamples)
                scheduler.getPool().release(&grain);
        });

        return output * 0.35f;
    }

    void advanceBlock(int numSamples, const SampleBuffer& sampleBuffer) noexcept
    {
        if (! useGranular || ! sampleBuffer.hasActiveSample())
            return;

        const auto data = sampleBuffer.getActive();
        const double blockDuration = static_cast<double>(numSamples) / sampleRate;
        scheduler.scheduleGrains(blockDuration,
                                 data->mono.data(),
                                 data->mono.size(),
                                 currentVelocity,
                                 rng);
    }

private:
    GranularParams params {};
    GrainScheduler scheduler {};
    GaussianWindow window {};
    SineOscillator sine {};
    std::mt19937 rng {};
    double sampleRate { 44100.0 };
    int currentNote { 60 };
    float currentVelocity { 0.8f };
    bool useGranular { false };
};
