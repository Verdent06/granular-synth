#include "AudioFileLoader.h"
#include "SampleBuffer.h"

#include <JuceHeader.h>

std::shared_ptr<const SampleData> AudioFileLoader::loadFile(const juce::File& file)
{
    if (! file.existsAsFile())
        return nullptr;

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return nullptr;

    const auto numSamples = static_cast<std::size_t>(reader->lengthInSamples);
    const auto numChannels = static_cast<std::size_t>(reader->numChannels);

    if (numSamples == 0 || numChannels == 0)
        return nullptr;

    juce::AudioBuffer<float> buffer(static_cast<int>(numChannels), static_cast<int>(numSamples));
    reader->read(&buffer, 0, static_cast<int>(numSamples), 0, true, true);

    auto data = std::make_shared<SampleData>();
    data->sampleRate = reader->sampleRate;
    data->mono.resize(numSamples);

    const float* left = buffer.getReadPointer(0);
    if (numChannels == 1)
    {
        for (std::size_t i = 0; i < numSamples; ++i)
            data->mono[i] = left[static_cast<int>(i)];
    }
    else
    {
        const float* right = buffer.getReadPointer(1);
        for (std::size_t i = 0; i < numSamples; ++i)
            data->mono[i] = 0.5f * (left[static_cast<int>(i)] + right[static_cast<int>(i)]);
    }

    return data;
}
