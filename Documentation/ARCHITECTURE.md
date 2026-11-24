# Architecture

- PluginProcessor: host integration, APVTS parameters, MIDI dispatch and versioned state.
- SynthVoice: four independent oscillator layers per voice, per-oscillator pitch/level/waveform, envelope and low-pass filter.
- SamplerEngine: 32 sample playback voices, sample-rate conversion, note tracking and sample import through JUCE AudioFormatManager.
- BasicEffects: post-instrument drive and stereo width.
- PluginEditor: native JUCE controls, oscillator layer panels, sample import, waveform overview and preset file actions.

## Realtime constraints

- Audio-thread code must not allocate, perform file I/O, block on locks, or access UI objects.
- Sample file decoding and waveform overview calculation occur in the editor/message-thread import path, not in the audio callback.
- Sample buffers are immutable after publication and retained for the lifetime of the processor so active sample voices never observe freed memory. Repeated imports therefore consume memory until the plugin instance is closed; the loader has a five-minute per-file limit.
- Parameter changes remain host-automatable through APVTS.
- State is serialized as XML with a schema version. User preset files use .craypreset; factory JSON presets are mapped by parameter ID.
- The sampler currently maps one imported sample from MIDI note 60. Keygroups, velocity layers, loop regions, trim points and root-note editing are not implemented yet.

## Validation

The Windows GitHub Actions workflow configures JUCE 8.0.6 and builds the VST3/Standalone targets in Release. CI success does not replace validation in FL Studio 26 and Cubase 13, nor audio-thread stress testing on a local Windows system.
