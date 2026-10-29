# Granular Synth

A polyphonic granular MIDI instrument built with JUCE. Load a WAV sample, trigger it from MIDI (FL Studio piano roll or keyboard), and sculpt a granular cloud with size, density, overlap, pitch, and pitch-scatter controls. Includes a global delay (time, feedback, mix).

## Features

- **MIDI instrument** — note-on/off with ADSR envelope and 16-voice polyphony (voice stealing)
- **Granular engine** — Gaussian-windowed grains, density/overlap scheduling, per-grain pitch scatter
- **Delay effect** — ring-buffer delay with feedback and wet/dry mix
- **Lock-free UI → audio** — SPSC FIFO with atomic head/tail (no mutexes on audio thread)
- **Custom memory pool** — slab allocator for grain slots; no heap allocation in `processBlock()`
- **Formats** — VST3 (FL Studio) + AU (Logic/GarageBand), universal macOS binary (arm64 + x86_64)

## Requirements

- macOS 11+
- Xcode Command Line Tools
- CMake 3.22+

## Build (Release)

```bash
cd "/Users/vedantdesai/Projects/FL Studio VST"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

Plugins are copied automatically to:

- VST3: `~/Library/Audio/Plug-Ins/VST3/Granular Synth.vst3`
- AU: `~/Library/Audio/Plug-Ins/Components/Granular Synth.component`

## Build (AddressSanitizer debug)

```bash
cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
  -DCMAKE_C_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address"
cmake --build build-asan -j8
```

Load the ASan-built plugin in [JUCE AudioPluginHost](https://github.com/juce-framework/JUCE/tree/master/extras/AudioPluginHost) to catch memory bugs.

## FL Studio

1. Rescan plugins: **Options → Manage plugins → Find more plugins**
2. Add **Granular Synth** to a **Channel** (instrument slot, not FX)
3. Click **Load Sample** in the plugin UI and pick a WAV file
4. Draw notes in the Piano Roll or play a MIDI keyboard

## Parameters

| Control | Description |
|---------|-------------|
| Delay Time | Echo delay in milliseconds |
| Delay Feedback | Amount of delayed signal fed back |
| Delay Mix | Wet/dry blend |
| Grain Size | Length of each grain (ms) |
| Density | Grains spawned per second |
| Overlap | How much grains overlap (0–0.99) |
| Pitch | Base pitch shift in semitones |
| Pitch Scatter | Random pitch offset range per grain |

## Real-time rules (`processBlock`)

**Never do these on the audio thread:**

- `new` / `delete` / `vector::push_back`
- File I/O or logging
- Mutex locks or blocking waits

All buffers, grain pools, and ring buffers are pre-allocated in `prepareToPlay()`. UI parameters arrive via the lock-free SPSC FIFO; WAV loading happens on the UI thread with atomic sample-buffer handoff.

## Project layout

```
Source/
  PluginProcessor.*     Audio entry, MIDI, param FIFO drain
  PluginEditor.*        Sliders, load-sample button
  dsp/                    Oscillator, ADSR, voices, delay, grains
  util/                   SPSC FIFO, memory pool, WAV loader, sample buffer
```

## Verify universal binary

```bash
lipo -info "build/GranularSynth_artefacts/Release/VST3/Granular Synth.vst3/Contents/MacOS/Granular Synth"
```

Expected: `Architectures in the fat file are: x86_64 arm64`
