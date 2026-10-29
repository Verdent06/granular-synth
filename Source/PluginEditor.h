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
    void setupSlider(juce::Slider& slider,
                     juce::Label& label,
                     const juce::String& text,
                     double min,
                     double max,
                     double step,
                     double defaultValue,
                     ParamId paramId);

    GranularSynthAudioProcessor& audioProcessor;

    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Slider delayMixSlider;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    juce::Label delayMixLabel;
    juce::TextButton loadSampleButton { "Load Sample" };
    juce::Label sampleNameLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthAudioProcessorEditor)
};
