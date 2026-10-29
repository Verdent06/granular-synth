cmake_minimum_required(VERSION 3.22)

project(GranularSynth VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(GranularSynth
    COMPANY_NAME "GranularSynth"
    BUNDLE_ID com.granularsynth.granularsynth
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE GrnS
    PLUGIN_CODE GrSy
    FORMATS VST3
    PRODUCT_NAME "GranularSynth"
    VST3_CATEGORIES Instrument Synth
)

juce_generate_juce_header(GranularSynth)

target_sources(GranularSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/dsp/SineOscillator.cpp
        Source/dsp/AdsrEnvelope.cpp
        Source/dsp/Voice.cpp
        Source/dsp/VoiceManager.cpp
        Source/dsp/DelayLine.cpp
        Source/dsp/GrainEngine.cpp
        Source/dsp/GrainScheduler.cpp
        Source/util/AudioFileLoader.cpp
)

target_compile_definitions(GranularSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(GranularSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
