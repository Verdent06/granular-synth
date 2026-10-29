#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

// Circular ring buffer with separate read/write heads and modulo wrap.
class RingBuffer
{
public:
    void prepare(std::size_t sizePowerOfTwo) noexcept
    {
        const std::size_t size = 1ULL << sizePowerOfTwo;
        buffer.assign(size, 0.0f);
        mask = size - 1;
        writeIndex = 0;
    }

    void reset() noexcept
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

    void write(float sample) noexcept
    {
        buffer[writeIndex & mask] = sample;
        ++writeIndex;
    }

    float readDelay(std::size_t delaySamples) const noexcept
    {
        if (buffer.empty())
            return 0.0f;

        const std::size_t readIndex = writeIndex - delaySamples;
        return buffer[readIndex & mask];
    }

    std::size_t size() const noexcept { return buffer.size(); }

private:
    std::vector<float> buffer;
    std::size_t mask { 0 };
    std::size_t writeIndex { 0 };
};
