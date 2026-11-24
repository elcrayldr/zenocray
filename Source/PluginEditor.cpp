#include "PluginEditor.h"

namespace {
const juce::Colour bg(0xff101114), panel(0xff1b1d22), yellow(0xffffc928), muted(0xff9a9da5);
void styleRotary(juce::Slider& s) {
 s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
 s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,66,18);
 s.setColour(juce::Slider::rotarySliderFillColourId,yellow);
 s.setColour(juce::Slider::thumbColourId,yellow);
 s.setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);
 s.setColour(juce::Slider::textBoxBackgroundColourId,panel);
 s.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
}
}

CRAYLDRUrbanInstrumentsAudioProcessorEditor::CRAYLDRUrbanInstrumentsAudioProcessorEditor(CRAYLDRUrbanInstrumentsAudioProcessor& p)
: AudioProcessorEditor(&p),processor(p) {
 setSize(1120,720);
 title.setText("CRAY LDR  /  URBAN INSTRUMENTS",juce::dontSendNotification);
 title.setFont(juce::Font(25.0f,juce::Font::bold));title.setColour(juce::Label::textColourId,yellow);addAndMakeVisible(title);
 subtitle.setText("FOUR-OSCILLATOR SYNTHESIS  •  64 VOICES",juce::dontSendNotification);
 subtitle.setFont(juce::Font(11.0f));subtitle.setColour(juce::Label::textColourId,muted);addAndMakeVisible(subtitle);

 const char* ids[]={"attack","decay","sustain","release","cutoff","resonance","level","drive","width"};
 const char* names[]={"ATTACK","DECAY","SUSTAIN","RELEASE","FILTER CUTOFF","RESONANCE","MASTER","DRIVE","STEREO WIDTH"};
 for(int i=0;i<9;++i){
  globalLabels[i].setText(names[i],juce::dontSendNotification);globalLabels[i].setJustificationType(juce::Justification::centred);
  globalLabels[i].setColour(juce::Label::textColourId,muted);addAndMakeVisible(globalLabels[i]);
  styleRotary(globalControls[i]);addAndMakeVisible(globalControls[i]);
  sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,ids[i],globalControls[i]));
 }

 for(int o=0;o<4;++o){
  const auto n=juce::String(o+1);
  const char* controlNames[]={"LEVEL","SEMITONES","FINE"};
  const juce::String paramIds[]={"osc"+n+"Level","osc"+n+"Semi","osc"+n+"Fine"};
  for(int c=0;c<3;++c){
   oscillatorLabels[o][c].setText(controlNames[c],juce::dontSendNotification);
   oscillatorLabels[o][c].setJustificationType(juce::Justification::centred);
   oscillatorLabels[o][c].setColour(juce::Label::textColourId,muted);addAndMakeVisible(oscillatorLabels[o][c]);
   styleRotary(oscillatorControls[o][c]);addAndMakeVisible(oscillatorControls[o][c]);
   sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,paramIds[c],oscillatorControls[o][c]));
  }
  oscillatorWaveforms[o].addItemList({"Sine","Saw","Square","Triangle","Noise"},1);
  oscillatorWaveforms[o].setColour(juce::ComboBox::backgroundColourId,panel);
  oscillatorWaveforms[o].setColour(juce::ComboBox::textColourId,juce::Colours::white);
  addAndMakeVisible(oscillatorWaveforms[o]);
  comboAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters,"osc"+n+"Wave",oscillatorWaveforms[o]));
 }
}

void CRAYLDRUrbanInstrumentsAudioProcessorEditor::paint(juce::Graphics& g){
 g.fillAll(bg);
 g.setColour(juce::Colour(0xff292c32));g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(10),12,1.2f);
 g.setColour(yellow);g.fillRoundedRectangle(24.0f,88.0f,(float)getWidth()-48.0f,2.0f,1.0f);
 g.setColour(panel);g.fillRoundedRectangle(20.0f,108.0f,(float)getWidth()-40.0f,190.0f,9.0f);
 g.fillRoundedRectangle(20.0f,320.0f,(float)getWidth()-40.0f,370.0f,9.0f);
 g.setColour(yellow);g.setFont(12.0f);g.drawText("GLOBAL VOICE / FILTER / OUTPUT",34,116,getWidth()-68,18,juce::Justification::left);
 g.drawText("OSCILLATOR LAYERING",34,328,getWidth()-68,18,juce::Justification::left);
 for(int i=0;i<4;++i){
  const int x=34+i*267;
  g.setColour(juce::Colour(0xff292c32));g.fillRoundedRectangle((float)x,354.0f,250.0f,320.0f,8.0f);
  g.setColour(yellow);g.setFont(juce::Font(17.0f,juce::Font::bold));g.drawText("OSC 0"+juce::String(i+1),x+12,366,220,24,juce::Justification::left);
 }
}

void CRAYLDRUrbanInstrumentsAudioProcessorEditor::resized(){
 title.setBounds(24,18,getWidth()-48,38);subtitle.setBounds(26,58,getWidth()-52,20);
 const int globalX=28,globalY=140,globalW=118,gap=4;
 for(int i=0;i<9;++i){const int x=globalX+i*(globalW+gap);globalLabels[i].setBounds(x,globalY,globalW,20);globalControls[i].setBounds(x,globalY+20,globalW,122);}
 for(int o=0;o<4;++o){
  const int x=42+o*267;
  for(int c=0;c<3;++c){const int cx=x+c*78;oscillatorLabels[o][c].setBounds(cx,414,74,18);oscillatorControls[o][c].setBounds(cx,432,74,112);}
  oscillatorWaveforms[o].setBounds(x+18,572,210,32);
 }
}