#pragma once

#include <array>
#include <cmath>
#include <cstddef>

class GaussianWindow
{
public:
    static constexpr std::size_t maxGrainSamples = 8192;

    void prepare(std::size_t maxLength) noexcept
    {
        const std::size_t clamped = std::min(maxLength, maxGrainSamples);
        for (std::size_t i = 0; i < clamped; ++i)
        {
            const double x = (static_cast<double>(i) / static_cast<double>(clamped - 1)) * 2.0 - 1.0;
            table[i] = static_cast<float>(std::exp(-4.0 * x * x));
        }
        preparedLength = clamped;
    }

    float lookup(std::size_t age, std::size_t lengthSamples) const noexcept
    {
        if (preparedLength == 0 || lengthSamples <= 1)
            return 1.0f;

        const std::size_t idx = (age * (preparedLength - 1)) / (lengthSamples - 1);
        return table[std::min(idx, preparedLength - 1)];
    }

private:
    std::array<float, maxGrainSamples> table {};
    std::size_t preparedLength { 0 };
};
