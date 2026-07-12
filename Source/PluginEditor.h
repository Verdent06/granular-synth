#pragma once

#include "PluginProcessor.h"

class GranularSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor&);
    ~GranularSynthAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthAudioProcessorEditor)
};
