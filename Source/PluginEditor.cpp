#include "PluginEditor.h"
CRAYLDRUrbanInstrumentsAudioProcessorEditor::CRAYLDRUrbanInstrumentsAudioProcessorEditor(CRAYLDRUrbanInstrumentsAudioProcessor& p)
: AudioProcessorEditor(&p),processor(p) {
    setSize(760,480);
    title.setText("CRAY LDR  /  URBAN INSTRUMENTS",juce::dontSendNotification);
    title.setFont(juce::Font(24.0f,juce::Font::bold)); title.setColour(juce::Label::textColourId,juce::Colour(0xffffc928)); addAndMakeVisible(title);
    const char* ids[]={"attack","decay","sustain","release","cutoff","resonance","level","drive"};
    const char* names[]={"ATTACK","DECAY","SUSTAIN","RELEASE","CUTOFF","RESONANCE","LEVEL","DRIVE"};
    for(int i=0;i<8;++i){
        labels[i].setText(names[i],juce::dontSendNotification); labels[i].setJustificationType(juce::Justification::centred);
        labels[i].setColour(juce::Label::textColourId,juce::Colours::white); addAndMakeVisible(labels[i]);
        controls[i].setSliderStyle(Slider::RotaryHorizontalVerticalDrag); controls[i].setTextBoxStyle(Slider::TextBoxBelow,false,76,20);
        controls[i].setColour(Slider::rotarySliderFillColourId,juce::Colour(0xffffc928)); addAndMakeVisible(controls[i]);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters,ids[i],controls[i]));
    }
    waveform.addItemList({"Sine","Saw","Square","Triangle","Noise"},1); addAndMakeVisible(waveform);
    waveform.setSelectedId(2); waveform.onChange=[this]{processor.parameters.getParameter("waveform")->setValueNotifyingHost((float)(waveform.getSelectedId()-1)/4.0f);};
}
void CRAYLDRUrbanInstrumentsAudioProcessorEditor::paint(juce::Graphics& g){
    g.fillAll(juce::Colour(0xff101114)); g.setColour(juce::Colour(0xff24262b)); g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(10),12,1.5f);
    g.setColour(juce::Colour(0xffffc928)); g.drawLine(24,76,(float)getWidth()-24,76,2.0f);
    g.setColour(juce::Colours::white.withAlpha(0.55f)); g.setFont(12); g.drawText("SYNTH ENGINE  /  OSCILLATOR 01",24,90,getWidth()-48,20,juce::Justification::left);
}
void CRAYLDRUrbanInstrumentsAudioProcessorEditor::resized(){
    title.setBounds(24,20,getWidth()-48,46);
    const int top=130,w=88,gap=5;
    for(int i=0;i<8;++i){const int x=22+i*(w+gap);labels[i].setBounds(x,top,w,22);controls[i].setBounds(x,top+22,w,112);}
    waveform.setBounds(24,300,180,32);
}
