#pragma once

#include "Voice.h"

#include <array>

class VoiceManager
{
public:
    static constexpr int maxVoices = 16;

    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        for (auto& voice : voices)
            voice.prepare(sampleRate);
    }

    void setUseGranular(bool enabled) noexcept
    {
        useGranular = enabled;
        for (auto& voice : voices)
            voice.setUseGranular(enabled);
    }

    void setGranularParams(const GranularParams& params) noexcept
    {
        granularParams = params;
        for (auto& voice : voices)
            voice.setGranularParams(params);
    }

    void noteOn(int noteNumber, float velocity, const SampleBuffer& sampleBuffer) noexcept
    {
        if (auto* voice = findFreeVoice())
        {
            voice->noteOn(noteNumber, velocity, sampleBuffer);
            return;
        }

        if (auto* stolen = stealOldestVoice())
            stolen->noteOn(noteNumber, velocity, sampleBuffer);
    }

    void noteOff(int noteNumber) noexcept
    {
        for (auto& voice : voices)
        {
            if (voice.isActive() && voice.getNote() == noteNumber)
                voice.noteOff();
        }
    }

    void renderBlock(float* output, int numSamples, const SampleBuffer& sampleBuffer) noexcept
    {
        for (int i = 0; i < numSamples; ++i)
            output[i] = 0.0f;

        for (auto& voice : voices)
            voice.advanceBlock(numSamples, sampleBuffer);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sum = 0.0f;
            for (auto& voice : voices)
                sum += voice.renderSample(sampleBuffer);

            output[sample] = sum;
        }
    }

private:
    Voice* findFreeVoice() noexcept
    {
        for (auto& voice : voices)
        {
            if (! voice.isActive())
                return &voice;
        }
        return nullptr;
    }

    Voice* stealOldestVoice() noexcept
    {
        Voice* oldest = &voices[0];
        for (auto& voice : voices)
        {
            if (voice.getAgeCounter() < oldest->getAgeCounter())
                oldest = &voice;
        }
        return oldest;
    }

    std::array<Voice, maxVoices> voices {};
    GranularParams granularParams {};
    double sampleRate { 44100.0 };
    bool useGranular { false };
};
