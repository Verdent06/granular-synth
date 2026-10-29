#pragma once

#include "util/ParamIds.h"
#include "util/SampleBuffer.h"
#include "util/SpscFifo.h"

#include "dsp/DelayLine.h"
#include "dsp/GrainScheduler.h"
#include "dsp/VoiceManager.h"

#include <JuceHeader.h>

#include <array>

class GranularSynthAudioProcessor : public juce::AudioProcessor
{
public:
    GranularSynthAudioProcessor();
    ~GranularSynthAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    ParamFifo& getParamFifo() noexcept { return paramFifo; }
    SampleBuffer& getSampleBuffer() noexcept { return sampleBuffer; }

    void queueParamChange(ParamId id, float value) noexcept;
    void loadSampleFromFile(const juce::File& file);

    GranularParams getGranularParams() const noexcept { return granularParams; }
    float getDelayTimeMs() const noexcept { return delayTimeMs; }
    float getDelayFeedback() const noexcept { return delayFeedback; }
    float getDelayMix() const noexcept { return delayMix; }

    juce::String getLoadedSampleName() const { return loadedSampleName; }

private:
    void applyParamUpdate(const ParamUpdate& update) noexcept;
    void drainParamFifo() noexcept;

    VoiceManager voiceManager;
    DelayLine delayLine;
    SampleBuffer sampleBuffer;
    ParamFifo paramFifo;

    GranularParams granularParams {};
    float delayTimeMs { 250.0f };
    float delayFeedback { 0.35f };
    float delayMix { 0.25f };

    std::array<float, 4096> tempRenderBuffer {};
    int maxBlockSize { 512 };
    juce::String loadedSampleName { "No sample loaded" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularSynthAudioProcessor)
};
