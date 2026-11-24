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
 juce::Label title,subtitle,sampleStatus;
 juce::TextButton loadSampleButton{"LOAD SAMPLE"},savePresetButton{"SAVE PRESET"},loadPresetButton{"LOAD PRESET"},clearSampleButton{"CLEAR ZONES"};
 juce::ComboBox factoryPresetSelector;
 std::unique_ptr<juce::FileChooser> presetChooser;
 std::unique_ptr<juce::FileChooser> sampleChooser;
 std::array<Slider,11> globalControls;
 std::array<juce::Label,11> globalLabels;
 std::array<std::array<Slider,3>,4> oscillatorControls;
 std::array<std::array<juce::Label,3>,4> oscillatorLabels;
 std::array<juce::ComboBox,4> oscillatorWaveforms;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRAYLDRUrbanInstrumentsAudioProcessorEditor)
};