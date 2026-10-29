#include "PluginEditor.h"

GranularSynthAudioProcessorEditor::GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(520, 360);
    setupSlider(delayTimeSlider, delayTimeLabel, "Delay Time (ms)", 1.0, 2000.0, 1.0, 250.0, ParamId::DelayTimeMs);
    setupSlider(delayFeedbackSlider, delayFeedbackLabel, "Delay Feedback", 0.0, 0.95, 0.01, 0.35, ParamId::DelayFeedback);
    setupSlider(delayMixSlider, delayMixLabel, "Delay Mix", 0.0, 1.0, 0.01, 0.25, ParamId::DelayMix);

    sampleNameLabel.setText(audioProcessor.getLoadedSampleName(), juce::dontSendNotification);
    addAndMakeVisible(sampleNameLabel);

    loadSampleButton.onClick = [this]() {
        fileChooser = std::make_unique<juce::FileChooser>("Select a WAV file", juce::File {}, "*.wav");
        const auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync(flags, [this](const juce::FileChooser& chooser) {
            if (const auto file = chooser.getResult(); file.existsAsFile())
            {
                audioProcessor.loadSampleFromFile(file);
                sampleNameLabel.setText(audioProcessor.getLoadedSampleName(), juce::dontSendNotification);
            }
        });
    };
    addAndMakeVisible(loadSampleButton);
}

void GranularSynthAudioProcessorEditor::setupSlider(juce::Slider& slider,
                                                    juce::Label& label,
                                                    const juce::String& text,
                                                    double min,
                                                    double max,
                                                    double step,
                                                    double defaultValue,
                                                    ParamId paramId)
{
    slider.setRange(min, max, step);
    slider.setValue(defaultValue, juce::dontSendNotification);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    slider.onValueChange = [this, paramId, &slider]() {
        audioProcessor.queueParamChange(paramId, static_cast<float>(slider.getValue()));
    };
    addAndMakeVisible(slider);
    label.setText(text, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
}

void GranularSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1e1e1e));
    g.setColour(juce::Colours::white);
    g.drawText("GranularSynth", getLocalBounds().removeFromTop(28), juce::Justification::centred);
}

void GranularSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(12);
    area.removeFromTop(28);
    sampleNameLabel.setBounds(area.removeFromTop(22));
    loadSampleButton.setBounds(area.removeFromTop(28).removeFromLeft(140));
    area.removeFromTop(8);
    delayTimeSlider.setBounds(area.removeFromTop(56));
    delayFeedbackSlider.setBounds(area.removeFromTop(56));
    delayMixSlider.setBounds(area.removeFromTop(56));
}
