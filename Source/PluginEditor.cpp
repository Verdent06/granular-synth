#include "PluginEditor.h"

GranularSynthAudioProcessorEditor::GranularSynthAudioProcessorEditor(GranularSynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
{
    setSize(480, 240);
}

void GranularSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1e1e1e));
    g.setColour(juce::Colours::white);
    g.drawText("GranularSynth", getLocalBounds(), juce::Justification::centred);
}

void GranularSynthAudioProcessorEditor::resized() {}
