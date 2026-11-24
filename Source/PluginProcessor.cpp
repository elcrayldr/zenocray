#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <BinaryData.h>

class FunctionThreadPoolJob final : public juce::ThreadPoolJob {
public:
    FunctionThreadPoolJob(juce::String name,std::function<void()> task)
        : juce::ThreadPoolJob(std::move(name)),work(std::move(task)) {}
    JobStatus runJob() override { work(); return jobHasFinished; }
private:
    std::function<void()> work;
};

static juce::AudioProcessorValueTreeState::ParameterLayout makeParams() {
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<AudioParameterFloat>("attack","Attack",NormalisableRange<float>(1.0f,2000.0f,1.0f),8.0f));
    p.add(std::make_unique<AudioParameterFloat>("decay","Decay",NormalisableRange<float>(1.0f,4000.0f,1.0f),180.0f));
    p.add(std::make_unique<AudioParameterFloat>("sustain","Sustain",0.0f,1.0f,0.78f));
    p.add(std::make_unique<AudioParameterFloat>("release","Release",NormalisableRange<float>(1.0f,5000.0f,1.0f),160.0f));
    p.add(std::make_unique<AudioParameterFloat>("cutoff","Cutoff",NormalisableRange<float>(20.0f,20000.0f,1.0f,0.25f),12000.0f));
    p.add(std::make_unique<AudioParameterFloat>("resonance","Resonance",0.1f,1.0f,0.15f));
    p.add(std::make_unique<AudioParameterFloat>("level","Master Level",0.0f,1.0f,0.2f));
    p.add(std::make_unique<AudioParameterFloat>("drive","Drive",0.0f,1.0f,0.0f));
    p.add(std::make_unique<AudioParameterFloat>("width","Stereo Width",0.0f,2.0f,1.0f));
    p.add(std::make_unique<AudioParameterFloat>("chorusMix","Chorus",0.0f,1.0f,0.0f));
    p.add(std::make_unique<AudioParameterFloat>("reverbMix","Reverb",0.0f,1.0f,0.0f));
    p.add(std::make_unique<AudioParameterFloat>("lfoRate","LFO Rate",NormalisableRange<float>(0.01f,20.0f,0.01f,0.35f),0.5f));
    p.add(std::make_unique<AudioParameterFloat>("lfoDepth","LFO Depth",0.0f,1.0f,0.0f));
    p.add(std::make_unique<AudioParameterChoice>("lfoDestination","LFO Destination",StringArray{"Off","Pitch","Filter","Amplitude"},0));
    p.add(std::make_unique<AudioParameterFloat>("samplerLevel","Sampler Level",0.0f,1.0f,0.8f));
    p.add(std::make_unique<AudioParameterFloat>("sampleRoot", "Sample Root Note", NormalisableRange<float>(0.0f,127.0f,1.0f),60.0f));
    p.add(std::make_unique<AudioParameterFloat>("sampleStart","Sample Start",0.0f,0.99f,0.0f));
    p.add(std::make_unique<AudioParameterFloat>("sampleEnd","Sample End",0.01f,1.0f,1.0f));
    p.add(std::make_unique<AudioParameterBool>("sampleLoop","Sample Loop",false));
    for (int i=1;i<=4;++i) {
        const auto suffix=String(i);
        p.add(std::make_unique<AudioParameterFloat>("osc"+suffix+"Level","OSC "+suffix+" Level",0.0f,1.0f,i==1?0.75f:0.0f));
        p.add(std::make_unique<AudioParameterFloat>("osc"+suffix+"Semi","OSC "+suffix+" Semitone",NormalisableRange<float>(-24.0f,24.0f,1.0f),0.0f));
        p.add(std::make_unique<AudioParameterFloat>("osc"+suffix+"Fine","OSC "+suffix+" Fine",NormalisableRange<float>(-100.0f,100.0f,0.1f),0.0f));
        p.add(std::make_unique<AudioParameterChoice>("osc"+suffix+"Wave","OSC "+suffix+" Wave",StringArray{"Sine","Saw","Square","Triangle","Noise"},i==1?1:0));
    }
    return p;
}


static bool applyJsonPreset(juce::AudioProcessorValueTreeState& state,const juce::String& jsonText,juce::String& error) {
    const auto parsed=juce::JSON::parse(jsonText);
    auto* root=parsed.getDynamicObject();
    if(root==nullptr){error="Invalid JSON preset.";return false;}
    const auto values=root->getProperty("parameters");
    auto* object=values.getDynamicObject();
    if(object==nullptr){error="Preset has no parameters object.";return false;}
    const char* ids[]={"attack","decay","sustain","release","cutoff","resonance","level","drive","width","chorusMix","reverbMix","lfoRate","lfoDepth","lfoDestination",
      "osc1Level","osc1Semi","osc1Fine","osc1Wave","osc2Level","osc2Semi","osc2Fine","osc2Wave",
      "osc3Level","osc3Semi","osc3Fine","osc3Wave","osc4Level","osc4Semi","osc4Fine","osc4Wave","samplerLevel","sampleRoot","sampleStart","sampleEnd","sampleLoop"};
    for(const auto* id:ids) {
        const auto value=object->getProperty(id);
        if(!value.isVoid()) {
            if(auto* parameter=dynamic_cast<juce::RangedAudioParameter*>(state.getParameter(id)))
                parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(value)));
        }
    }
    error.clear();
    return true;
}

CRAYLDRUrbanInstrumentsAudioProcessor::CRAYLDRUrbanInstrumentsAudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output",AudioChannelSet::stereo(),true)),
  parameters(*this,nullptr,"CRAY_STATE",makeParams()) {
    for(int i=0;i<static_cast<int>(synthVoices.size());++i){synthVoices[static_cast<size_t>(i)]=new cray::SynthVoice();synth.addVoice(synthVoices[static_cast<size_t>(i)]);}
    synth.addSound(new cray::SynthSound());
    cached.attack=parameters.getRawParameterValue("attack");
    cached.decay=parameters.getRawParameterValue("decay");
    cached.sustain=parameters.getRawParameterValue("sustain");
    cached.release=parameters.getRawParameterValue("release");
    cached.cutoff=parameters.getRawParameterValue("cutoff");
    cached.resonance=parameters.getRawParameterValue("resonance");
    cached.masterLevel=parameters.getRawParameterValue("level");
    cached.drive=parameters.getRawParameterValue("drive");
    cached.width=parameters.getRawParameterValue("width");
    cached.chorusMix=parameters.getRawParameterValue("chorusMix");
    cached.reverbMix=parameters.getRawParameterValue("reverbMix");
    cached.lfoRate=parameters.getRawParameterValue("lfoRate");
    cached.lfoDepth=parameters.getRawParameterValue("lfoDepth");
    cached.lfoDestination=parameters.getRawParameterValue("lfoDestination");
    cached.samplerLevel=parameters.getRawParameterValue("samplerLevel");
    cached.sampleRoot=parameters.getRawParameterValue("sampleRoot");
    cached.sampleStart=parameters.getRawParameterValue("sampleStart");
    cached.sampleEnd=parameters.getRawParameterValue("sampleEnd");
    cached.sampleLoop=parameters.getRawParameterValue("sampleLoop");
    for(int osc=0;osc<4;++osc){
        const auto n=juce::String(osc+1);
        cached.oscillatorLevel[static_cast<size_t>(osc)]=parameters.getRawParameterValue("osc"+n+"Level");
        cached.oscillatorSemitone[static_cast<size_t>(osc)]=parameters.getRawParameterValue("osc"+n+"Semi");
        cached.oscillatorFine[static_cast<size_t>(osc)]=parameters.getRawParameterValue("osc"+n+"Fine");
        cached.oscillatorWave[static_cast<size_t>(osc)]=parameters.getRawParameterValue("osc"+n+"Wave");
    }
}

CRAYLDRUrbanInstrumentsAudioProcessor::~CRAYLDRUrbanInstrumentsAudioProcessor() {
    sampleLoader.removeAllJobs(true,-1);
}

void CRAYLDRUrbanInstrumentsAudioProcessor::loadSampleFromFileAsync(const juce::File& file,std::function<void(bool,juce::String)> completion) {
    sampler.setRootNote(static_cast<int>(parameters.getRawParameterValue("sampleRoot")->load()));
    auto* job=new FunctionThreadPoolJob("CRAY LDR sample import",[this,file,completion=std::move(completion)]() mutable {
        juce::String error;
        const bool success=sampler.loadSampleFromFile(file,error);
        juce::MessageManager::callAsync([completion=std::move(completion),success,error]() mutable {
            if(completion) completion(success,error);
        });
    });
    sampleLoader.addJob(job,true);
}

void CRAYLDRUrbanInstrumentsAudioProcessor::prepareToPlay(double sr,int block) {
    synth.setCurrentPlaybackSampleRate(sr);
    for(auto* voice:synthVoices) if(voice!=nullptr) voice->prepare(sr);
    effects.prepare(sr,block,getTotalNumOutputChannels());
    sampler.prepare(sr);
}

bool CRAYLDRUrbanInstrumentsAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    return l.getMainOutputChannelSet()==AudioChannelSet::mono() || l.getMainOutputChannelSet()==AudioChannelSet::stereo();
}

void CRAYLDRUrbanInstrumentsAudioProcessor::processBlock(AudioBuffer<float>& b,MidiBuffer& m) {
    ScopedNoDenormals noDenormals;
    for(int c=getTotalNumInputChannels();c<getTotalNumOutputChannels();++c) b.clear(c,0,b.getNumSamples());
    const auto load=[](const std::atomic<float>* value){return value->load(std::memory_order_relaxed);};
    for(auto* voice:synthVoices) {
        if(voice!=nullptr) {
            voice->setParameters(load(cached.attack),load(cached.decay),load(cached.sustain),load(cached.release),
                                 load(cached.cutoff),load(cached.resonance),load(cached.masterLevel),load(cached.lfoRate),load(cached.lfoDepth),static_cast<int>(load(cached.lfoDestination)));
            for(int osc=0;osc<4;++osc) {
                const auto index=static_cast<size_t>(osc);
                voice->setOscillator(osc,load(cached.oscillatorLevel[index]),load(cached.oscillatorSemitone[index]),
                                     load(cached.oscillatorFine[index]),static_cast<int>(load(cached.oscillatorWave[index])));
            }
        }
    }
    b.clear();
    synth.renderNextBlock(b,m,0,b.getNumSamples());
    sampler.setLevel(load(cached.samplerLevel));
    sampler.setRootNote(static_cast<int>(load(cached.sampleRoot)));
    sampler.setTrim(load(cached.sampleStart),load(cached.sampleEnd));
    sampler.setLoop(load(cached.sampleLoop)>0.5f);
    sampler.processBlock(b,m);
    effects.setDrive(load(cached.drive));
    effects.setWidth(load(cached.width));
    effects.setChorusMix(load(cached.chorusMix));
    effects.setReverbMix(load(cached.reverbMix));
    effects.process(b);
}

juce::AudioProcessorEditor* CRAYLDRUrbanInstrumentsAudioProcessor::createEditor(){return new CRAYLDRUrbanInstrumentsAudioProcessorEditor(*this);}
void CRAYLDRUrbanInstrumentsAudioProcessor::getStateInformation(MemoryBlock& dest) {
    auto state=parameters.copyState();state.setProperty("schemaVersion",2,nullptr);
    std::unique_ptr<XmlElement> xml(state.createXml());copyXmlToBinary(*xml,dest);
}
void CRAYLDRUrbanInstrumentsAudioProcessor::setStateInformation(const void* data,int size) {
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data,size));
    if(xml&&xml->hasTagName(parameters.state.getType()))parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::File CRAYLDRUrbanInstrumentsAudioProcessor::getUserPresetDirectory() const {
    auto dir=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                 .getChildFile("CRAY LDR").getChildFile("Urban Instruments").getChildFile("Presets");
    dir.createDirectory();
    return dir;
}

bool CRAYLDRUrbanInstrumentsAudioProcessor::savePresetToFile(const juce::File& file,juce::String& error) {
    auto state=parameters.copyState();
    state.setProperty("schemaVersion",2,nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if(xml==nullptr||!xml->writeTo(file)){error="Could not write preset file.";return false;}
    error.clear();
    return true;
}

bool CRAYLDRUrbanInstrumentsAudioProcessor::loadPresetFromFile(const juce::File& file,juce::String& error) {
    if(!file.existsAsFile()){error="Preset file does not exist.";return false;}
    if(file.hasFileExtension("json")) return applyJsonPreset(parameters,file.loadFileAsString(),error);
    auto xml=juce::XmlDocument::parse(file.loadFileAsString());
    if(xml==nullptr||!xml->hasTagName(parameters.state.getType())){error="Invalid CRAY LDR preset file.";return false;}
    auto state=juce::ValueTree::fromXml(*xml);
    if(!state.isValid()){error="Preset state is invalid.";return false;}
    parameters.replaceState(state);
    error.clear();
    return true;
}

bool CRAYLDRUrbanInstrumentsAudioProcessor::loadFactoryPreset(int index,juce::String& error) {
    if(index==1) return applyJsonPreset(parameters,juce::String::fromUTF8(BinaryData::Init_Sub_Sine_json,BinaryData::Init_Sub_Sine_jsonSize),error);
    if(index==2) return applyJsonPreset(parameters,juce::String::fromUTF8(BinaryData::Init_Urban_Saw_json,BinaryData::Init_Urban_Saw_jsonSize),error);
    error="Unknown factory preset.";
    return false;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new CRAYLDRUrbanInstrumentsAudioProcessor();}
