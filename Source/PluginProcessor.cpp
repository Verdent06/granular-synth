#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "util/AudioFileLoader.h"

GranularSynthAudioProcessor::GranularSynthAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void GranularSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    maxBlockSize = samplesPerBlock;
    voiceManager.prepare(sampleRate);
    delayLine.prepare(sampleRate);
    delayLine.setDelayTimeMs(delayTimeMs);
    delayLine.setFeedback(delayFeedback);
    delayLine.setMix(delayMix);
    voiceManager.setGranularParams(granularParams);
}

void GranularSynthAudioProcessor::releaseResources()
{
}

void GranularSynthAudioProcessor::queueParamChange(ParamId id, float value) noexcept
{
    paramFifo.push({ id, value });
}

void GranularSynthAudioProcessor::loadSampleFromFile(const juce::File& file)
{
    if (auto data = AudioFileLoader::loadFile(file))
    {
        sampleBuffer.setPending(std::move(data));
        loadedSampleName = file.getFileName();
        voiceManager.setUseGranular(true);
    }
}

void GranularSynthAudioProcessor::applyParamUpdate(const ParamUpdate& update) noexcept
{
    switch (update.id)
    {
        case ParamId::DelayTimeMs:
            delayTimeMs = update.value;
            delayLine.setDelayTimeMs(delayTimeMs);
            break;
        case ParamId::DelayFeedback:
            delayFeedback = update.value;
            delayLine.setFeedback(delayFeedback);
            break;
        case ParamId::DelayMix:
            delayMix = update.value;
            delayLine.setMix(delayMix);
            break;
        case ParamId::GrainSizeMs:
            granularParams.grainSizeMs = update.value;
            voiceManager.setGranularParams(granularParams);
            break;
        case ParamId::GrainDensity:
            granularParams.density = update.value;
            voiceManager.setGranularParams(granularParams);
            break;
        case ParamId::GrainOverlap:
            granularParams.overlap = update.value;
            voiceManager.setGranularParams(granularParams);
            break;
        case ParamId::GrainPitchSemitones:
            granularParams.pitchSemitones = update.value;
            voiceManager.setGranularParams(granularParams);
            break;
        case ParamId::GrainPitchScatter:
            granularParams.pitchScatter = update.value;
            voiceManager.setGranularParams(granularParams);
            break;
        case ParamId::Count:
            break;
    }
}

void GranularSynthAudioProcessor::drainParamFifo() noexcept
{
    ParamUpdate updates[32];
    std::size_t count = 0;
    paramFifo.drain(updates, 32, count);

    for (std::size_t i = 0; i < count; ++i)
        applyParamUpdate(updates[i]);
}

void GranularSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    sampleBuffer.swapPendingToActive();
    drainParamFifo();

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
            voiceManager.noteOn(message.getNoteNumber(), message.getFloatVelocity(), sampleBuffer);
        else if (message.isNoteOff())
            voiceManager.noteOff(message.getNoteNumber());
    }

    float* renderTarget = tempRenderBuffer.data();
    const int renderSamples = juce::jmin(numSamples, maxBlockSize, static_cast<int>(tempRenderBuffer.size()));
    voiceManager.renderBlock(renderTarget, renderSamples, sampleBuffer);

    for (int sample = 0; sample < renderSamples; ++sample)
        renderTarget[sample] = delayLine.processSample(renderTarget[sample]);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < renderSamples; ++sample)
            channelData[sample] = renderTarget[sample];
    }
}

juce::AudioProcessorEditor* GranularSynthAudioProcessor::createEditor()
{
    return new GranularSynthAudioProcessorEditor(*this);
}

void GranularSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state("GranularSynth");
    state.setProperty("delayTimeMs", delayTimeMs, nullptr);
    state.setProperty("delayFeedback", delayFeedback, nullptr);
    state.setProperty("delayMix", delayMix, nullptr);
    state.setProperty("grainSizeMs", granularParams.grainSizeMs, nullptr);
    state.setProperty("grainDensity", granularParams.density, nullptr);
    state.setProperty("grainOverlap", granularParams.overlap, nullptr);
    state.setProperty("grainPitch", granularParams.pitchSemitones, nullptr);
    state.setProperty("grainScatter", granularParams.pitchScatter, nullptr);
    state.setProperty("sampleName", loadedSampleName, nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void GranularSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        const juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        delayTimeMs = state.getProperty("delayTimeMs", delayTimeMs);
        delayFeedback = state.getProperty("delayFeedback", delayFeedback);
        delayMix = state.getProperty("delayMix", delayMix);
        granularParams.grainSizeMs = state.getProperty("grainSizeMs", granularParams.grainSizeMs);
        granularParams.density = state.getProperty("grainDensity", granularParams.density);
        granularParams.overlap = state.getProperty("grainOverlap", granularParams.overlap);
        granularParams.pitchSemitones = state.getProperty("grainPitch", granularParams.pitchSemitones);
        granularParams.pitchScatter = state.getProperty("grainScatter", granularParams.pitchScatter);
        loadedSampleName = state.getProperty("sampleName", loadedSampleName).toString();

        delayLine.setDelayTimeMs(delayTimeMs);
        delayLine.setFeedback(delayFeedback);
        delayLine.setMix(delayMix);
        voiceManager.setGranularParams(granularParams);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularSynthAudioProcessor();
}
