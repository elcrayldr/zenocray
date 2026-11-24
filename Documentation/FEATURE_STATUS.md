# Feature status — independent development

| Area | Status | Notes |
|---|---|---|
| JUCE 8.0.6 / C++17 / Windows x64 VST3 | Implemented; latest full build under validation | Windows CI builds VST3 and Standalone |
| Four independent oscillators | Implemented in DSP | Per-oscillator waveform, level, semitone and fine tuning; 64 synth voices |
| Oscillator anti-aliasing | Partial | PolyBLEP on saw and square; triangle remains naïve |
| ADSR / low-pass filter | Basic implementation | Shared per-voice envelope and low-pass |
| Per-voice LFO | Implemented | Sine LFO to pitch, filter cutoff or amplitude |
| Drive / stereo width / chorus / reverb | Basic FX rack implemented | Host-automatable mix controls; delay and advanced routing pending |
| Polyphonic sample playback | Basic multisample implementation | 32 voices; up to 16 samples, nearest-root-note zone selection |
| WAV / AIFF / FLAC import | Implemented via JUCE basic formats | Dedicated worker; 5-minute per-file and 512 MB retained-memory limits |
| Sample waveform overview | Implemented | Peak overview of most recently imported zone |
| Sample start/end trim and loop | Implemented | Normalized start/end points and loop toggle; non-destructive |
| Sample root mapping | Basic implementation | Root note selected before import; nearest root zone plays |
| Factory presets | Embedded selector implemented | Init Sub Sine and Init Urban Saw |
| User preset save/load | Implemented | Versioned APVTS XML .craypreset files; external JSON import supported |
| Velocity layers / round-robin / explicit zone ranges | Not implemented | Planned |
| Modulation matrix / tempo-sync LFO | Not implemented | Planned |
| Delay / advanced FX routing | Not implemented | Planned |
| FL Studio 26 / Cubase 13 validation | Pending | CI compilation is not DAW validation |
| Realtime stress / long-session testing | Pending | Must be completed before release |

A successful CI build only proves that the source compiles in the configured Windows environment; it does not establish host stability, sound quality, latency, or release readiness.
