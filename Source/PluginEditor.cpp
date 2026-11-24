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
 setSize(1120,970);
 title.setText("CRAY LDR  /  URBAN INSTRUMENTS",juce::dontSendNotification);
 title.setFont(juce::Font(juce::FontOptions(25.0f,juce::Font::bold)));title.setColour(juce::Label::textColourId,yellow);addAndMakeVisible(title);
 subtitle.setText("FOUR-OSCILLATOR SYNTHESIS  •  64 VOICES",juce::dontSendNotification);
 subtitle.setFont(juce::Font(juce::FontOptions(11.0f)));subtitle.setColour(juce::Label::textColourId,muted);addAndMakeVisible(subtitle);
 loadSampleButton.setColour(juce::TextButton::buttonColourId,yellow);loadSampleButton.setColour(juce::TextButton::textColourOffId,bg);addAndMakeVisible(loadSampleButton);
 sampleStatus.setText(processor.getLoadedSampleName(),juce::dontSendNotification);sampleStatus.setColour(juce::Label::textColourId,muted);sampleStatus.setFont(juce::Font(juce::FontOptions(10.0f)));addAndMakeVisible(sampleStatus);
 factoryPresetSelector.addItem("INIT SUB SINE",1);factoryPresetSelector.addItem("INIT URBAN SAW",2);
 factoryPresetSelector.setTextWhenNothingSelected("FACTORY PRESETS");
 factoryPresetSelector.setColour(juce::ComboBox::backgroundColourId,panel);factoryPresetSelector.setColour(juce::ComboBox::textColourId,juce::Colours::white);addAndMakeVisible(factoryPresetSelector);
 factoryPresetSelector.onChange=[this]{if(factoryPresetSelector.getSelectedId()>0){juce::String error;if(!processor.loadFactoryPreset(factoryPresetSelector.getSelectedId(),error))sampleStatus.setText(error,juce::dontSendNotification);else sampleStatus.setText("FACTORY PRESET: "+factoryPresetSelector.getText(),juce::dontSendNotification);}};

 savePresetButton.setColour(juce::TextButton::buttonColourId,panel);savePresetButton.setColour(juce::TextButton::textColourOffId,yellow);addAndMakeVisible(savePresetButton);
 loadPresetButton.setColour(juce::TextButton::buttonColourId,panel);loadPresetButton.setColour(juce::TextButton::textColourOffId,yellow);addAndMakeVisible(loadPresetButton);
 clearSampleButton.setColour(juce::TextButton::buttonColourId,panel);clearSampleButton.setColour(juce::TextButton::textColourOffId,yellow);addAndMakeVisible(clearSampleButton);
 clearSampleButton.onClick=[this]{processor.clearSampleZones();sampleStatus.setText("ZONES CLEARED — MEMORY RELEASES WHEN PLUGIN CLOSES",juce::dontSendNotification);repaint();};
 savePresetButton.onClick=[this]{
  presetChooser=std::make_unique<juce::FileChooser>("Save CRAY LDR preset",processor.getUserPresetDirectory().getChildFile("New Preset.craypreset"),"*.craypreset");
  juce::Component::SafePointer<CRAYLDRUrbanInstrumentsAudioProcessorEditor> safeThis(this);
  presetChooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles,[safeThis](const juce::FileChooser& chooser){
   if(safeThis==nullptr)return;
   auto file=chooser.getResult();if(file!=juce::File{}){if(file.getFileExtension().isEmpty())file=juce::File(file.getFullPathName()+".craypreset");juce::String error;if(!safeThis->processor.savePresetToFile(file,error))safeThis->sampleStatus.setText(error,juce::dontSendNotification);else safeThis->sampleStatus.setText("PRESET SAVED: "+file.getFileName(),juce::dontSendNotification);}safeThis->presetChooser.reset();
  });
 };
 loadPresetButton.onClick=[this]{
  presetChooser=std::make_unique<juce::FileChooser>("Load CRAY LDR preset",processor.getUserPresetDirectory(),"*.craypreset;*.xml;*.json");
  juce::Component::SafePointer<CRAYLDRUrbanInstrumentsAudioProcessorEditor> safeThis(this);
  presetChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safeThis](const juce::FileChooser& chooser){
   if(safeThis==nullptr)return;
   const auto file=chooser.getResult();if(file.existsAsFile()){juce::String error;if(!safeThis->processor.loadPresetFromFile(file,error))safeThis->sampleStatus.setText(error,juce::dontSendNotification);else safeThis->sampleStatus.setText("PRESET LOADED: "+file.getFileName(),juce::dontSendNotification);}safeThis->presetChooser.reset();
  });
 };
 loadSampleButton.onClick=[this]{
  sampleChooser=std::make_unique<juce::FileChooser>("Load audio sample",juce::File{},"*.wav;*.aif;*.aiff;*.flac");
  juce::Component::SafePointer<CRAYLDRUrbanInstrumentsAudioProcessorEditor> safeThis(this);
  sampleChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safeThis](const juce::FileChooser& chooser){
   if(safeThis==nullptr)return;
   const auto file=chooser.getResult();
   if(file.existsAsFile()){
    safeThis->sampleStatus.setText("LOADING SAMPLE...",juce::dontSendNotification);
    safeThis->processor.loadSampleFromFileAsync(file,[safeThis,file](bool success,juce::String error){
     if(safeThis!=nullptr){safeThis->sampleStatus.setText(success?file.getFileName():error,juce::dontSendNotification);safeThis->repaint();}
    });
   }
   safeThis->sampleChooser.reset();
  });
 };

 const char* ids[]={"attack","decay","sustain","release","cutoff","resonance","level","drive","width","chorusMix","reverbMix","samplerLevel","sampleRoot"};
 const char* names[]={"ATTACK","DECAY","SUSTAIN","RELEASE","FILTER CUTOFF","RESONANCE","MASTER","DRIVE","STEREO WIDTH","CHORUS","REVERB","SAMPLE LEVEL","NEXT ROOT"};
 for(int i=0;i<13;++i){
  globalLabels[i].setText(names[i],juce::dontSendNotification);globalLabels[i].setJustificationType(juce::Justification::centred);
  globalLabels[i].setColour(juce::Label::textColourId,muted);addAndMakeVisible(globalLabels[i]);
  styleRotary(globalControls[i]);addAndMakeVisible(globalControls[i]);
  sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,ids[i],globalControls[i]));
 }

 const char* lfoNames[]={"RATE","DEPTH"};
 const char* lfoIds[]={"lfoRate","lfoDepth"};
 for(int i=0;i<2;++i){
  lfoLabels[i].setText(lfoNames[i],juce::dontSendNotification);lfoLabels[i].setJustificationType(juce::Justification::centred);
  lfoLabels[i].setColour(juce::Label::textColourId,muted);addAndMakeVisible(lfoLabels[i]);
  styleRotary(lfoControls[i]);addAndMakeVisible(lfoControls[i]);
  sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,lfoIds[i],lfoControls[i]));
 }
 lfoDestinationLabel.setText("DESTINATION",juce::dontSendNotification);lfoDestinationLabel.setColour(juce::Label::textColourId,muted);lfoDestinationLabel.setJustificationType(juce::Justification::centred);addAndMakeVisible(lfoDestinationLabel);
 lfoDestinationSelector.addItemList({"Off","Pitch","Filter","Amplitude"},1);
 lfoDestinationSelector.setColour(juce::ComboBox::backgroundColourId,panel);lfoDestinationSelector.setColour(juce::ComboBox::textColourId,juce::Colours::white);addAndMakeVisible(lfoDestinationSelector);
 comboAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters,"lfoDestination",lfoDestinationSelector));
 sampleStartLabel.setText("START",juce::dontSendNotification);sampleStartLabel.setColour(juce::Label::textColourId,muted);sampleStartLabel.setJustificationType(juce::Justification::centred);addAndMakeVisible(sampleStartLabel);
 sampleEndLabel.setText("END",juce::dontSendNotification);sampleEndLabel.setColour(juce::Label::textColourId,muted);sampleEndLabel.setJustificationType(juce::Justification::centred);addAndMakeVisible(sampleEndLabel);
 for(auto* slider:{&sampleStartSlider,&sampleEndSlider}){slider->setSliderStyle(juce::Slider::LinearHorizontal);slider->setTextBoxStyle(juce::Slider::TextBoxRight,false,40,18);slider->setColour(juce::Slider::thumbColourId,yellow);slider->setColour(juce::Slider::trackColourId,yellow);slider->setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);slider->setColour(juce::Slider::textBoxBackgroundColourId,panel);addAndMakeVisible(*slider);}
 sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,"sampleStart",sampleStartSlider));
 sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,"sampleEnd",sampleEndSlider));
 sampleLoopToggle.setButtonText("LOOP");sampleLoopToggle.setColour(juce::ToggleButton::textColourId,juce::Colours::white);addAndMakeVisible(sampleLoopToggle);
 buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters,"sampleLoop",sampleLoopToggle));

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
 g.fillRoundedRectangle(20.0f,710.0f,(float)getWidth()-40.0f,118.0f,9.0f);
 g.fillRoundedRectangle(20.0f,840.0f,(float)getWidth()-40.0f,118.0f,9.0f);
 g.setColour(yellow);g.setFont(12.0f);g.drawText("GLOBAL VOICE / FILTER / OUTPUT",34,116,getWidth()-68,18,juce::Justification::left);
 g.drawText("OSCILLATOR LAYERING",34,328,getWidth()-68,18,juce::Justification::left);
 g.drawText("LFO MODULATION",34,718,getWidth()-68,18,juce::Justification::left);
 g.drawText("SAMPLE EDITOR / WAVEFORM OVERVIEW",34,848,getWidth()-68,18,juce::Justification::left);
 const auto& preview=processor.getSamplePreview();
 const float center=915.0f, halfHeight=34.0f, x0=330.0f, usableWidth=static_cast<float>(getWidth())-366.0f;
 g.setColour(juce::Colour(0xff343840));g.drawLine(x0,center,x0+usableWidth,center,1.0f);
 g.setColour(yellow);
 for(size_t i=0;i<preview.size();++i){const float x=x0+usableWidth*static_cast<float>(i)/static_cast<float>(preview.size()-1);const float h=juce::jlimit(0.0f,1.0f,preview[i])*halfHeight;g.drawLine(x,center-h,x,center+h,1.2f);}
 for(int i=0;i<4;++i){
  const int x=34+i*267;
  g.setColour(juce::Colour(0xff292c32));g.fillRoundedRectangle((float)x,354.0f,250.0f,320.0f,8.0f);
  g.setColour(yellow);g.setFont(juce::Font(juce::FontOptions(17.0f,juce::Font::bold)));g.drawText("OSC 0"+juce::String(i+1),x+12,366,220,24,juce::Justification::left);
 }
}

void CRAYLDRUrbanInstrumentsAudioProcessorEditor::resized(){
 factoryPresetSelector.setBounds(410,766,205,30);
 sampleStartLabel.setBounds(32,874,94,18);sampleStartSlider.setBounds(30,892,100,34);
 sampleEndLabel.setBounds(138,874,94,18);sampleEndSlider.setBounds(136,892,100,34);
 sampleLoopToggle.setBounds(240,890,82,36);
 lfoLabels[0].setBounds(34,744,82,18);lfoControls[0].setBounds(30,760,90,62);
 lfoLabels[1].setBounds(130,744,82,18);lfoControls[1].setBounds(126,760,90,62);
 lfoDestinationLabel.setBounds(236,744,150,18);lfoDestinationSelector.setBounds(236,766,150,30);
 title.setBounds(24,18,590,38);subtitle.setBounds(26,58,590,20);savePresetButton.setBounds(getWidth()-494,26,112,32);loadPresetButton.setBounds(getWidth()-374,26,112,32);loadSampleButton.setBounds(getWidth()-254,26,132,32);clearSampleButton.setBounds(getWidth()-114,26,90,32);sampleStatus.setBounds(getWidth()-494,62,470,20);
 const int globalX=34,globalY=140,globalW=76,gap=4;
 for(int i=0;i<13;++i){const int x=globalX+i*(globalW+gap);globalLabels[i].setBounds(x,globalY,globalW,20);globalControls[i].setBounds(x,globalY+20,globalW,122);}
 for(int o=0;o<4;++o){
  const int x=42+o*267;
  for(int c=0;c<3;++c){const int cx=x+c*78;oscillatorLabels[o][c].setBounds(cx,414,74,18);oscillatorControls[o][c].setBounds(cx,432,74,112);}
  oscillatorWaveforms[o].setBounds(x+18,572,210,32);
 }
}