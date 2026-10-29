#pragma once

#include <cstdint>

enum class ParamId : uint8_t
{
    DelayTimeMs = 0,
    DelayFeedback,
    DelayMix,
    GrainSizeMs,
    GrainDensity,
    GrainOverlap,
    GrainPitchSemitones,
    GrainPitchScatter,
    Count
};

struct ParamUpdate
{
    ParamId id {};
    float value {};
};
