# Feature status — independent development

| Area | Status | Notes |
|---|---|---|
| JUCE 8.0.6 / C++17 / Windows x64 VST3 | Implemented; CI build passed on earlier milestone | Latest sampler-zone, async-import and factory-browser changes are under CI validation |
| Four independent oscillators | Implemented in DSP | Per-oscillator waveform, level, semitone and fine tuning; 64 synth voices |
| Oscillator anti-aliasing | Partial | PolyBLEP on saw and square; triangle remains naïve |
| ADSR / low-pass filter | Basic implementation | Shared per-voice envelope and low-pass |
| Polyphonic sample playback | Basic multisample implementation | 32 voices; up to 16 samples, nearest-root-note zone selection |
| WAV / AIFF / FLAC import | Implemented via JUCE basic formats | Dedicated worker; 5-minute per-file and 512 MB retained-memory limits |
| Sample waveform overview | Implemented | Peak overview of most recently imported zone; no trim or loop editing |
| Sample root mapping | Basic implementation | Root note is selected before import; nearest root zone plays |
| Factory presets | Embedded selector implemented | Init Sub Sine and Init Urban Saw |
| User preset save/load | Implemented | Versioned APVTS XML .craypreset files; external JSON import supported |
| Stereo width / drive | Basic implementation | Zero drive preserves unity; no advanced FX rack yet |
| Velocity layers / round-robin / zone range editor | Not implemented | Planned |
| Sample trim and loop points | Not implemented | Planned |
| Modulation matrix / LFOs / effects rack | Not implemented | Planned |
| FL Studio 26 / Cubase 13 validation | Pending | CI compilation is not DAW validation |
| Realtime stress / long-session testing | Pending | Must be completed before release |

A successful CI build only proves that the source compiles in the configured Windows environment; it does not establish host stability, sound quality, latency, or release readiness.
