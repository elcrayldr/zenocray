#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class CRAYLDRUrbanInstrumentsAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
    explicit CRAYLDRUrbanInstrumentsAudioProcessorEditor(CRAYLDRUrbanInstrumentsAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    using Slider=juce::Slider;
    CRAYLDRUrbanInstrumentsAudioProcessor& processor;
    juce::Label title;
    std::array<Slider,8> controls;
    std::array<juce::Label,8> labels;
    juce::ComboBox waveform;
    juce::Slider drive,width;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRAYLDRUrbanInstrumentsAudioProcessorEditor)
};
