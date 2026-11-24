# CRAY LDR URBAN INSTRUMENTS

Independent implementation of a Windows x64 virtual instrument using JUCE 8.0.6 and C++17.

## Implemented in the current development line

- 64-voice subtractive synth with four independent oscillators.
- Per-oscillator waveform, level, semitone and fine tuning.
- ADSR envelope, low-pass filter, drive and stereo width.
- Polyphonic one-shot sample layer with 32 voices.
- WAV, AIFF and FLAC import using JUCE AudioFormatManager.
- Sample waveform overview in the editor.
- User preset save/load (.craypreset) and factory JSON preset loading.
- VST3 and Standalone targets.

## Current limitations

The sampler currently maps one imported sample across the keyboard with MIDI note 60 as its root. Keygroups, velocity layers, loop and trim editing, root-note controls, a modulation matrix, LFOs and a full effects rack remain in development. Sample imports are decoded from the editor thread and old sample buffers are retained until the plugin instance closes to guarantee safe audio-thread ownership.

This is a development prototype, not a release-ready product. The Windows CI build is a compile check; FL Studio 26 and Cubase 13 validation and realtime stress testing remain outstanding.

## Build

Requires Visual Studio 2022 (Desktop development with C++), CMake 3.22+, and an internet connection for the pinned JUCE FetchContent dependency.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```
