# CRAY LDR URBAN INSTRUMENTS

Independent implementation of a Windows x64 virtual instrument using JUCE 8.0.6 and C++17.

## Implemented in the current development line

- 64-voice subtractive synth with four independent oscillators.
- Per-oscillator waveform, level, semitone and fine tuning.
- ADSR envelope, low-pass filter, drive and stereo width.
- Polyphonic one-shot sample layer with 32 voices.
- WAV, AIFF and FLAC import using JUCE AudioFormatManager.
- Adjustable sample root note and sampler output level.
- Sample waveform overview in the editor.
- User preset save/load (.craypreset) and factory JSON preset loading.
- VST3 and Standalone targets.

## Current limitations

The sampler currently maps one imported sample across the keyboard from a selectable root note. Keygroups, velocity layers, loop and trim editing, a modulation matrix, LFOs and a full effects rack remain in development. Sample imports are decoded on a dedicated background worker. Replaced sample buffers are retained until the plugin instance closes to guarantee safe audio-thread ownership, with a 512 MB retained-sample budget per plugin instance and a five-minute per-file limit.

This is a development prototype, not a release-ready product. Windows CI compilation is not equivalent to validation in FL Studio 26 and Cubase 13; host compatibility and realtime stress testing remain outstanding.

## Build

Requires Visual Studio 2022 (Desktop development with C++), CMake 3.22+, and an internet connection for the pinned JUCE FetchContent dependency.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```
