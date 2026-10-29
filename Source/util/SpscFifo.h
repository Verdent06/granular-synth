#pragma once

#include "ParamIds.h"

#include <array>
#include <atomic>
#include <cstddef>

// Single-producer (UI), single-consumer (audio) lock-free FIFO.
template <typename T, std::size_t Capacity>
class SpscFifo
{
public:
    static_assert(Capacity > 1, "Capacity must be greater than 1");

    bool push(const T& item) noexcept
    {
        const auto currentTail = tail.load(std::memory_order_relaxed);
        const auto nextTail = (currentTail + 1) % Capacity;

        if (nextTail == head.load(std::memory_order_acquire))
            return false;

        buffer[currentTail] = item;
        tail.store(nextTail, std::memory_order_release);
        return true;
    }

    bool pop(T& item) noexcept
    {
        const auto currentHead = head.load(std::memory_order_relaxed);

        if (currentHead == tail.load(std::memory_order_acquire))
            return false;

        item = buffer[currentHead];
        head.store((currentHead + 1) % Capacity, std::memory_order_release);
        return true;
    }

    void drain(T* outValues, std::size_t maxCount, std::size_t& countOut) noexcept
    {
        countOut = 0;
        T item;

        while (countOut < maxCount && pop(item))
        {
            outValues[countOut++] = item;
        }
    }

private:
    std::array<T, Capacity> buffer {};
    std::atomic<std::size_t> head { 0 };
    std::atomic<std::size_t> tail { 0 };
};

using ParamFifo = SpscFifo<ParamUpdate, 64>;
