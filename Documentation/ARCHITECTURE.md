# Architecture

- PluginProcessor: host integration, APVTS parameters, MIDI dispatch, versioned state and background sample-load job queue.
- SynthVoice: four independent oscillator layers per voice, per-oscillator pitch/level/waveform, envelope, low-pass filter and per-voice LFO modulation to pitch/filter/amplitude.
- SamplerEngine: 32 sample playback voices, up to 16 imported root-note zones, nearest-root selection, sample-rate conversion and MIDI note tracking.
- BasicEffects: post-instrument drive, stereo width, chorus and reverb.
- PluginEditor: native JUCE controls, oscillator layer panels, async sample import, waveform overview, embedded factory preset selector and user preset file actions.

## Realtime constraints

- Audio-thread code must not allocate, perform file I/O, block on locks, or access UI objects.
- Sample decoding and waveform overview calculation run on a dedicated worker. Preview publication uses a short UI-side critical section; the audio callback only reads immutable sample buffers through published atomic pointers.
- Sample buffers are immutable after publication and retained for the lifetime of the processor so active sample voices never observe freed memory. Repeated imports consume memory until the plugin instance closes; limits are 16 zones, five minutes per file and 512 MB retained sample data per plugin instance.
- The sampler captures the selected root note when each sample is imported. MIDI notes use the sample zone with the nearest root note; velocity layers and explicit zone boundaries are not implemented.
- APVTS raw parameter atomics and voice pointers are cached before audio processing; the callback performs no parameter-ID string lookups.
- Parameter changes remain host-automatable through APVTS.
- State is serialized as XML with a schema version. User preset files use .craypreset; factory JSON resources are embedded and mapped by parameter ID.

## Validation

The Windows GitHub Actions workflow configures JUCE 8.0.6 and builds the VST3/Standalone targets in Release, then runs portable DSP sanity tests. CI success does not replace validation in FL Studio 26 and Cubase 13, nor audio-thread stress testing on a local Windows system.
