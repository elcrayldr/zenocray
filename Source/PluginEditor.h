#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class CRAYLDRUrbanInstrumentsAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
 explicit CRAYLDRUrbanInstrumentsAudioProcessorEditor(CRAYLDRUrbanInstrumentsAudioProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
 void timerCallback() override;
private:
 using Slider=juce::Slider;
 CRAYLDRUrbanInstrumentsAudioProcessor& processor;
 juce::Label title,subtitle,sampleStatus,lfoDestinationLabel;
 juce::String lastSampleName;
 juce::TextButton loadSampleButton{"LOAD SAMPLE"},savePresetButton{"SAVE PRESET"},loadPresetButton{"LOAD PRESET"},clearSampleButton{"CLEAR ZONES"};
 juce::ComboBox factoryPresetSelector;
 std::unique_ptr<juce::FileChooser> presetChooser;
 std::unique_ptr<juce::FileChooser> sampleChooser;
 std::array<Slider,13> globalControls;
 std::array<juce::Label,13> globalLabels;
 std::array<Slider,2> lfoControls;
 Slider sampleStartSlider,sampleEndSlider;
 juce::Label sampleStartLabel,sampleEndLabel;
 juce::ToggleButton sampleLoopToggle;
 std::array<juce::Label,2> lfoLabels;
 juce::ComboBox lfoDestinationSelector;
 std::array<std::array<Slider,3>,4> oscillatorControls;
 std::array<std::array<juce::Label,3>,4> oscillatorLabels;
 std::array<juce::ComboBox,4> oscillatorWaveforms;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRAYLDRUrbanInstrumentsAudioProcessorEditor)
};