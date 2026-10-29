#pragma once

#include <algorithm>
#include <cmath>

enum class AdsrStage
{
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
};

class AdsrEnvelope
{
public:
    void prepare(double sampleRateIn) noexcept
    {
        sampleRate = sampleRateIn;
        recalculateRates();
    }

    void setAttackSeconds(float seconds) noexcept
    {
        attackSeconds = std::max(0.001f, seconds);
        recalculateRates();
    }

    void setDecaySeconds(float seconds) noexcept
    {
        decaySeconds = std::max(0.001f, seconds);
        recalculateRates();
    }

    void setSustainLevel(float sustainLevelIn) noexcept
    {
        sustainLevel = std::clamp(sustainLevelIn, 0.0f, 1.0f);
        recalculateRates();
    }

    void setReleaseSeconds(float seconds) noexcept
    {
        releaseSeconds = std::max(0.001f, seconds);
        recalculateRates();
    }

    void noteOn() noexcept
    {
        stage = AdsrStage::Attack;
    }

    void noteOff() noexcept
    {
        if (stage != AdsrStage::Idle)
            stage = AdsrStage::Release;
    }

    void reset() noexcept
    {
        stage = AdsrStage::Idle;
        level = 0.0f;
    }

    bool isActive() const noexcept
    {
        return stage != AdsrStage::Idle;
    }

    float processSample() noexcept
    {
        switch (stage)
        {
            case AdsrStage::Attack:
                level += attackRate;
                if (level >= 1.0f)
                {
                    level = 1.0f;
                    stage = AdsrStage::Decay;
                }
                break;

            case AdsrStage::Decay:
                level -= decayRate;
                if (level <= sustainLevel)
                {
                    level = sustainLevel;
                    stage = AdsrStage::Sustain;
                }
                break;

            case AdsrStage::Sustain:
                break;

            case AdsrStage::Release:
                level -= releaseRate;
                if (level <= 0.0f)
                {
                    level = 0.0f;
                    stage = AdsrStage::Idle;
                }
                break;

            case AdsrStage::Idle:
            default:
                level = 0.0f;
                break;
        }

        return level;
    }

private:
    void recalculateRates() noexcept
    {
        attackRate = 1.0f / static_cast<float>(attackSeconds * sampleRate);
        decayRate = (1.0f - sustainLevel) / static_cast<float>(decaySeconds * sampleRate);
        releaseRate = sustainLevel / static_cast<float>(releaseSeconds * sampleRate);
    }

    double sampleRate { 44100.0 };
    AdsrStage stage { AdsrStage::Idle };
    float level { 0.0f };
    float attackSeconds { 0.01f };
    float decaySeconds { 0.1f };
    float sustainLevel { 0.7f };
    float releaseSeconds { 0.2f };
    float attackRate { 0.0f };
    float decayRate { 0.0f };
    float releaseRate { 0.0f };
};
