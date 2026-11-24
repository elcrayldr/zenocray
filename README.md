# CRAY LDR URBAN INSTRUMENTS

Independent implementation of a Windows x64 virtual instrument using JUCE 8.0.6 and C++17.

## Implemented in the current development line

- 64-voice subtractive synth with four independent oscillators.
- Per-oscillator waveform, level, semitone and fine tuning.
- ADSR envelope, low-pass filter, per-voice LFO, drive, stereo width, chorus and reverb.
- Polyphonic sample layer with 32 voices and up to 16 root-note zones.
- WAV, AIFF and FLAC import on a dedicated background worker.
- Nearest-root-note sample mapping, adjustable root note for each imported zone, and sampler output level.
- Sample waveform overview for the most recently imported zone.
- Embedded factory presets and user preset save/load (.craypreset).
- VST3 and Standalone targets.

## Current limitations

The sampler selects the imported sample whose root note is nearest to the incoming MIDI note. It does not yet provide explicit zone boundaries, velocity layers, round-robin, loop points or destructive trim editing. Replaced sample buffers are retained until the plugin instance closes to guarantee safe audio-thread ownership, with a 512 MB retained-sample budget, a 16-zone limit and a five-minute per-file limit.

This is a development prototype, not a release-ready product. Windows CI compilation and portable DSP sanity tests are not equivalent to validation in FL Studio 26 and Cubase 13; host compatibility and realtime stress testing remain outstanding.

## Build

Requires Visual Studio 2022 (Desktop development with C++), CMake 3.22+, and an internet connection for the pinned JUCE FetchContent dependency.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```
