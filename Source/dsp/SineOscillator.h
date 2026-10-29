#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

class SineOscillator
{
public:
    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
    }

    void setFrequency(float frequencyHz) noexcept
    {
        frequency = frequencyHz;
    }

    void reset() noexcept
    {
        phase = 0.0;
    }

    float processSample() noexcept
    {
        static constexpr double twoPi = 6.283185307179586;
        const float output = static_cast<float>(std::sin(twoPi * phase));
        phase += static_cast<double>(frequency) / sampleRate;
        if (phase >= 1.0)
            phase -= 1.0;
        return output;
    }

    static float midiNoteToFrequency(int noteNumber) noexcept
    {
        return 440.0f * std::pow(2.0f, (static_cast<float>(noteNumber) - 69.0f) / 12.0f);
    }

private:
    double sampleRate { 44100.0 };
    double phase { 0.0 };
    float frequency { 440.0f };
};
