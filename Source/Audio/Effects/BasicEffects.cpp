#include "BasicEffects.h"
#include <cmath>

namespace cray {
void BasicEffects::prepare(double sampleRate,int maxBlock,int channels) {
    const juce::dsp::ProcessSpec spec{sampleRate,static_cast<juce::uint32>(juce::jmax(1,maxBlock)),
                                      static_cast<juce::uint32>(juce::jmax(1,channels))};
    chorus.prepare(spec);
    chorus.setRate(0.35f);
    chorus.setDepth(0.22f);
    chorus.setCentreDelay(8.0f);
    chorus.setFeedback(0.0f);
    chorus.setMix(chorusMix);
    reverb.setSampleRate(sampleRate);
    reverbParameters=reverb.getParameters();
}

void BasicEffects::process(juce::AudioBuffer<float>& buffer) noexcept {
    const int numSamples=buffer.getNumSamples();
    const int numChannels=buffer.getNumChannels();
    if(numSamples<=0||numChannels<=0)return;

    const float saturationMix=drive;
    const float saturationGain=1.0f+saturationMix*8.0f;
    const float normalisation=std::tanh(saturationGain);

    if(numChannels==1) {
        auto* mono=buffer.getWritePointer(0);
        for(int i=0;i<numSamples;++i) {
            const float dry=mono[i];
            const float saturated=std::tanh(dry*saturationGain)/normalisation;
            mono[i]=dry+(saturated-dry)*saturationMix;
        }
    } else {
        auto* left=buffer.getWritePointer(0);
        auto* right=buffer.getWritePointer(1);
        for(int i=0;i<numSamples;++i) {
            const float mid=(left[i]+right[i])*0.5f;
            const float side=(left[i]-right[i])*0.5f*width;
            const float widthLeft=mid+side;
            const float widthRight=mid-side;
            const float satLeft=std::tanh(widthLeft*saturationGain)/normalisation;
            const float satRight=std::tanh(widthRight*saturationGain)/normalisation;
            left[i]=widthLeft+(satLeft-widthLeft)*saturationMix;
            right[i]=widthRight+(satRight-widthRight)*saturationMix;
        }
    }

    if(numChannels>=2) {
        chorus.setMix(chorusMix);
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        chorus.process(context);
    }

    {
        reverbParameters.roomSize=0.55f;
        reverbParameters.damping=0.42f;
        reverbParameters.wetLevel=0.33f*reverbMix;
        reverbParameters.dryLevel=0.5f*(1.0f-reverbMix);
        reverbParameters.width=1.0f;
        reverbParameters.freezeMode=0.0f;
        reverb.setParameters(reverbParameters);
        if(numChannels>=2)reverb.processStereo(buffer.getWritePointer(0),buffer.getWritePointer(1),numSamples);
        else reverb.processMono(buffer.getWritePointer(0),numSamples);
    }
}
}
