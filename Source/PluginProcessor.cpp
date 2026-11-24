#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout makeParams() {
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<AudioParameterFloat>("attack","Attack",NormalisableRange<float>(1,2000,1),8));
    p.add(std::make_unique<AudioParameterFloat>("decay","Decay",NormalisableRange<float>(1,4000,1),180));
    p.add(std::make_unique<AudioParameterFloat>("sustain","Sustain",0.0f,1.0f,0.78f));
    p.add(std::make_unique<AudioParameterFloat>("release","Release",NormalisableRange<float>(1,5000,1),160));
    p.add(std::make_unique<AudioParameterFloat>("cutoff","Cutoff",NormalisableRange<float>(20,20000,1,0.25f),12000));
    p.add(std::make_unique<AudioParameterFloat>("resonance","Resonance",0.1f,1.0f,0.15f));
    p.add(std::make_unique<AudioParameterFloat>("level","Level",0.0f,1.0f,0.2f));
    p.add(std::make_unique<AudioParameterChoice>("waveform","Waveform",StringArray{"Sine","Saw","Square","Triangle","Noise"},1));
    p.add(std::make_unique<AudioParameterFloat>("drive","Drive",0.0f,1.0f,0.0f));
    p.add(std::make_unique<AudioParameterFloat>("width","Width",0.0f,2.0f,1.0f));
    return p;
}
CRAYLDRUrbanInstrumentsAudioProcessor::CRAYLDRUrbanInstrumentsAudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output",AudioChannelSet::stereo(),true)),
  parameters(*this,nullptr,"CRAY_STATE",makeParams()) {
    for(int i=0;i<64;++i) synth.addVoice(new cray::SynthVoice());
    synth.addSound(new cray::SynthSound());
}
void CRAYLDRUrbanInstrumentsAudioProcessor::prepareToPlay(double sr,int block) {
    synth.setCurrentPlaybackSampleRate(sr);
    for(int i=0;i<synth.getNumVoices();++i)
        if(auto* v=dynamic_cast<cray::SynthVoice*>(synth.getVoice(i))) v->prepare(sr);
    effects.prepare(sr,block,getTotalNumOutputChannels());
}
bool CRAYLDRUrbanInstrumentsAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    return l.getMainOutputChannelSet()==AudioChannelSet::mono() || l.getMainOutputChannelSet()==AudioChannelSet::stereo();
}
void CRAYLDRUrbanInstrumentsAudioProcessor::processBlock(AudioBuffer<float>& b,MidiBuffer& m) {
    ScopedNoDenormals noDenormals;
    for(int c=getTotalNumInputChannels();c<getTotalNumOutputChannels();++c) b.clear(c,0,b.getNumSamples());
    const auto get=[this](const char* id){return parameters.getRawParameterValue(id)->load();};
    const int wave=static_cast<int>(get("waveform"));
    for(int i=0;i<synth.getNumVoices();++i)
        if(auto* v=dynamic_cast<cray::SynthVoice*>(synth.getVoice(i)))
            v->setParameters(get("attack"),get("decay"),get("sustain"),get("release"),get("cutoff"),get("resonance"),get("level"),wave);
    b.clear(); synth.renderNextBlock(b,m,0,b.getNumSamples());
    effects.setDrive(get("drive")); effects.setWidth(get("width")); effects.process(b);
}
juce::AudioProcessorEditor* CRAYLDRUrbanInstrumentsAudioProcessor::createEditor(){return new CRAYLDRUrbanInstrumentsAudioProcessorEditor(*this);}
void CRAYLDRUrbanInstrumentsAudioProcessor::getStateInformation(MemoryBlock& dest) {
    auto state=parameters.copyState(); state.setProperty("schemaVersion",1,nullptr);
    std::unique_ptr<XmlElement> xml(state.createXml()); copyXmlToBinary(*xml,dest);
}
void CRAYLDRUrbanInstrumentsAudioProcessor::setStateInformation(const void* data,int size) {
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data,size));
    if(xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new CRAYLDRUrbanInstrumentsAudioProcessor();}
