#pragma once

#include "AdsrEnvelope.h"
#include "GrainEngine.h"

#include "../util/SampleBuffer.h"

#include <atomic>
#include <cstdint>

class Voice
{
public:
    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        engine.prepare(sampleRate);
        envelope.prepare(sampleRate);
    }

    void setUseGranular(bool enabled) noexcept
    {
        engine.setUseGranular(enabled);
    }

    void setGranularParams(const GranularParams& params) noexcept
    {
        engine.setParams(params);
    }

    void noteOn(int noteNumber, float velocity, const SampleBuffer& sampleBuffer) noexcept
    {
        note = noteNumber;
        velocityLevel = velocity;
        ageCounter = ++globalVoiceCounter;
        active = true;
        envelope.noteOn();
        engine.noteOn(noteNumber, velocity, sampleBuffer);
    }

    void noteOff() noexcept
    {
        envelope.noteOff();
    }

    bool isActive() const noexcept
    {
        return active;
    }

    bool isReleasing() const noexcept
    {
        return active && envelope.isActive();
    }

    std::uint64_t getAgeCounter() const noexcept
    {
        return ageCounter;
    }

    int getNote() const noexcept
    {
        return note;
    }

    float renderSample(const SampleBuffer& sampleBuffer) noexcept
    {
        if (! active)
            return 0.0f;

        const float env = envelope.processSample();
        const float source = engine.processSample(sampleBuffer);
        const float output = source * env * velocityLevel;

        if (! envelope.isActive())
        {
            active = false;
            engine.reset();
        }

        return output;
    }

    void advanceBlock(int numSamples, const SampleBuffer& sampleBuffer) noexcept
    {
        if (active)
            engine.advanceBlock(numSamples, sampleBuffer);
    }

    static inline std::atomic<std::uint64_t> globalVoiceCounter { 0 };

private:
    GrainEngine engine {};
    AdsrEnvelope envelope {};
    double sampleRate { 44100.0 };
    int note { -1 };
    float velocityLevel { 0.0f };
    bool active { false };
    std::uint64_t ageCounter { 0 };
};
