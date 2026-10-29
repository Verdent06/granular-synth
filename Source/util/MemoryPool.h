#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Fixed-size slab allocator for grain slots. No heap allocation after prepare().
template <typename T, std::size_t Capacity>
class MemoryPool
{
public:
    MemoryPool() noexcept
    {
        for (std::size_t i = 0; i < Capacity; ++i)
            freeIndices[i] = static_cast<std::int32_t>(Capacity - 1 - i);

        freeCount = static_cast<std::int32_t>(Capacity);
    }

    T* acquire() noexcept
    {
        if (freeCount <= 0)
            return nullptr;

        const auto index = freeIndices[static_cast<std::size_t>(--freeCount)];
        slots[static_cast<std::size_t>(index)] = T {};
        active[static_cast<std::size_t>(index)] = true;
        return &slots[static_cast<std::size_t>(index)];
    }

    void release(T* slot) noexcept
    {
        if (slot == nullptr)
            return;

        const auto index = static_cast<std::size_t>(slot - slots.data());
        if (index >= Capacity || ! active[static_cast<std::size_t>(index)])
            return;

        active[static_cast<std::size_t>(index)] = false;
        freeIndices[static_cast<std::size_t>(freeCount++)] = static_cast<std::int32_t>(index);
    }

    template <typename Fn>
    void forEachActive(Fn&& fn) noexcept
    {
        for (std::size_t i = 0; i < Capacity; ++i)
        {
            if (active[i])
                fn(slots[i]);
        }
    }

    std::size_t capacity() const noexcept { return Capacity; }

private:
    std::array<T, Capacity> slots {};
    std::array<bool, Capacity> active {};
    std::array<std::int32_t, Capacity> freeIndices {};
    std::int32_t freeCount { 0 };
};
