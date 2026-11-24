# Feature status — independent development

| Area | Status | Notes |
|---|---|---|
| JUCE 8.0.6 / C++17 / Windows x64 VST3 | Implemented; CI build passed on baseline | Current feature changes are under fresh CI validation |
| Four independent oscillators | Implemented in DSP | Per-oscillator waveform, level, semitone and fine tuning; 64 synth voices |
| Oscillator anti-aliasing | Partial | PolyBLEP on saw and square; triangle remains naïve |
| ADSR / low-pass filter | Basic implementation | Shared per-voice envelope and low-pass |
| Polyphonic sample playback | Basic implementation | 32 sample voices, one loaded sample mapped across keyboard from C4 |
| WAV / AIFF / FLAC import | Implemented via JUCE basic formats | Import is initiated from the editor; large files may temporarily block the UI |
| Sample waveform overview | Implemented | Peak overview; not yet a destructive editor |
| Factory JSON presets | Loader implemented | Factory examples use the current APVTS parameter IDs |
| User preset save/load | Implemented | Versioned APVTS XML .craypreset files |
| Stereo width / drive | Basic implementation | Zero drive preserves unity; no advanced FX rack yet |
| Sample zones / keygroups / velocity layers | Not implemented | Planned sampler milestone |
| Sample trim, loop points, root-note editor | Not implemented | Planned |
| Modulation matrix / LFOs / effects rack | Not implemented | Planned |
| FL Studio 26 / Cubase 13 validation | Pending | CI compilation is not DAW validation |
| Realtime stress / long-session testing | Pending | Must be completed before release |

This is an actively developed prototype. A successful CI build only proves that the source compiles in the configured Windows environment; it does not establish host stability, sound quality, latency, or release readiness.
