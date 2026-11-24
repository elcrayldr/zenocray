#pragma once
#include <JuceHeader.h>
#include <array>
namespace cray {
class SynthSound final : public juce::SynthesiserSound {
public:
 bool appliesToNote(int) override { return true; }
 bool appliesToChannel(int) override { return true; }
};
class SynthVoice final : public juce::SynthesiserVoice {
public:
 bool canPlaySound(juce::SynthesiserSound*) override;
 void startNote(int,float,juce::SynthesiserSound*,int) override;
 void stopNote(float,bool) override;
 void pitchWheelMoved(int) override;
 void controllerMoved(int,int) override {}
 void renderNextBlock(juce::AudioBuffer<float>&,int,int) override;
 void prepare(double);
 void setParameters(float,float,float,float,float,float,float);
 void setOscillator(int,float,float,float,int) noexcept;
private:
 static constexpr int oscillatorCount=4;
 double sr=44100.0;
 double phases[oscillatorCount]{0.0,0.0,0.0,0.0};
 double baseIncrements[oscillatorCount]{0.0,0.0,0.0,0.0};
 float oscillatorLevels[oscillatorCount]{0.75f,0.0f,0.0f,0.0f};
 float oscillatorSemitones[oscillatorCount]{0.0f,0.0f,0.0f,0.0f};
 float oscillatorCents[oscillatorCount]{0.0f,0.0f,0.0f,0.0f};
 int oscillatorWaveforms[oscillatorCount]{1,0,0,0};
 float velocityGain=0.0f,pitchRatio=1.0f;
 float attackMs=8.0f,decayMs=180.0f,sustain=0.78f,releaseMs=160.0f;
 float cutoffHz=12000.0f,resonance=0.15f,masterLevel=0.2f;
 juce::ADSR envelope;
 juce::ADSR::Parameters envParams;
 juce::dsp::StateVariableTPTFilter<float> filter;
 juce::Random random;
 float renderOscillator(int) noexcept;
 static float polyBlep(float,float) noexcept;
};
}