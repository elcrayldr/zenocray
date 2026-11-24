# Architecture

- PluginProcessor: host integration, MIDI dispatch, APVTS parameters and state.
- SynthVoice: per-voice oscillator/envelope/filter rendering.
- BasicEffects: post-synth drive and stereo width stage.
- PluginEditor: native JUCE controls and host parameter attachments.

## Engineering constraints

Audio-thread code must not allocate, perform file I/O, block on locks, or access UI objects. Sample loading and preset parsing must be implemented on worker threads and transferred to the audio engine through bounded, ownership-safe handoff. Parameter changes must remain host-automatable. State format must be versioned and tolerate unknown/missing properties.
