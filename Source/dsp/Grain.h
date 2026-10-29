#pragma once

#include <cstddef>

struct Grain
{
    double startSample { 0.0 };
    double playbackRate { 1.0 };
    double readPosition { 0.0 };
    std::size_t lengthSamples { 0 };
    std::size_t age { 0 };
    float amplitude { 1.0f };
    bool active { false };
};
