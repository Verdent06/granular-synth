#pragma once

#include <JuceHeader.h>

#include <memory>

struct SampleData;

class AudioFileLoader
{
public:
    static std::shared_ptr<const SampleData> loadFile(const juce::File& file);
};
