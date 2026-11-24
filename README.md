# CRAY LDR URBAN INSTRUMENTS

Independent development line for a Windows x64 VST3 instrument built with JUCE 8.0.6 and C++17.

## Current status

This repository is being initialized from the independent 0.1.0 development baseline. The current milestone is an architectural/audio-engine prototype, not a release-ready instrument. It has not yet been validated in FL Studio or Cubase.

## Planned scope

- Four-oscillator subtractive synthesis engine
- Multisample sampler (WAV/AIFF/FLAC)
- Factory and user libraries
- Preset management and versioned plugin state
- Sample editor and instrument builder
- Modulation and effects
- Native JUCE interface
- Windows x64 VST3 and standalone builds

## Build

Requires Visual Studio 2022 (Desktop development with C++), CMake 3.22+, and a working internet connection for the pinned JUCE FetchContent dependency.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The VST3 target is generated under the build tree. Exact output paths depend on the CMake generator.

## Important

Do not treat the current baseline as production-ready. Real-time safety, sampler loading, full preset serialization, parameter coverage, UI workflows, and host compatibility require further implementation and testing.