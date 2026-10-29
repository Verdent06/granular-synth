#pragma once

#include <atomic>
#include <memory>
#include <vector>

struct SampleData
{
    std::vector<float> mono;
    double sampleRate { 44100.0 };
};

// Lock-free sample handoff using atomic shared_ptr operations (C++11).
class SampleBuffer
{
public:
    void setPending(std::shared_ptr<const SampleData> data) noexcept
    {
        std::atomic_store_explicit(&pending, std::move(data), std::memory_order_release);
    }

    void swapPendingToActive() noexcept
    {
        if (auto next = std::atomic_exchange_explicit(&pending,
                                                      std::shared_ptr<const SampleData> {},
                                                      std::memory_order_acq_rel))
        {
            std::atomic_store_explicit(&active, std::move(next), std::memory_order_release);
        }
    }

    std::shared_ptr<const SampleData> getActive() const noexcept
    {
        return std::atomic_load_explicit(&active, std::memory_order_acquire);
    }

    bool hasActiveSample() const noexcept
    {
        const auto data = getActive();
        return data != nullptr && ! data->mono.empty();
    }

private:
    std::shared_ptr<const SampleData> pending { nullptr };
    std::shared_ptr<const SampleData> active { nullptr };
};
