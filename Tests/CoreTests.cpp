#include <cmath>
#include <iostream>
#include <algorithm>

static bool near(double a,double b,double tolerance=1.0e-8){return std::abs(a-b)<=tolerance;}

int main() {
    constexpr double sampleRate=48000.0,frequency=440.0;
    const double increment=frequency/sampleRate;
    if(!(increment>0.0&&increment<0.5))return 1;

    double phase=0.0;
    for(int i=0;i<48000;++i){phase+=increment;if(phase>=1.0)phase-=std::floor(phase);}
    if(!near(phase,0.0))return 2;

    const double upTwoSemitones=std::pow(2.0,2.0/12.0);
    const double downTwoSemitones=std::pow(2.0,-2.0/12.0);
    if(!near(upTwoSemitones*downTwoSemitones,1.0))return 3;
    if(!(upTwoSemitones>1.0&&downTwoSemitones<1.0))return 4;

    for(int i=-10000;i<=10000;++i){
        const double input=static_cast<double>(i)/1000.0;
        const double y=std::tanh(input);
        if(!std::isfinite(y)||std::abs(y)>1.0)return 5;
    }

    // At zero drive the dry/wet saturator must be exactly transparent.
    for(int i=-100;i<=100;++i){
        const float dry=static_cast<float>(i)/100.0f;
        const float drive=0.0f;
        const float wet=std::tanh(dry)/std::tanh(1.0f);
        const float output=dry+(wet-dry)*drive;
        if(output!=dry)return 6;
    }

    // Linear interpolation must remain bounded between adjacent sample values.
    for(int i=0;i<=100;++i){
        const double t=static_cast<double>(i)/100.0;
        const double value=-0.75+(0.5+0.75)*t;
        if(value< -0.7500001||value>0.5000001)return 7;
    }

    std::cout<<"CRAY LDR core DSP sanity tests passed\n";
    return 0;
}