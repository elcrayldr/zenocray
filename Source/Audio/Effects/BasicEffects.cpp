#include "BasicEffects.h"
#include <cmath>
namespace cray {
void BasicEffects::prepare(double, int, int) {}
void BasicEffects::process(juce::AudioBuffer<float>& b) noexcept {
    const int n = b.getNumSamples();
    if (b.getNumChannels() == 1) {
        auto* x = b.getWritePointer(0);
        const float gain = 1.0f + drive * 8.0f;
        for (int i=0;i<n;++i) x[i] = std::tanh(x[i] * gain) / std::tanh(gain);
        return;
    }
    auto* l=b.getWritePointer(0); auto* r=b.getWritePointer(1);
    const float gain=1.0f+drive*8.0f, norm=std::tanh(gain);
    for(int i=0;i<n;++i) {
        const float mid=(l[i]+r[i])*0.5f, side=(l[i]-r[i])*0.5f*width;
        l[i]=std::tanh((mid+side)*gain)/norm;
        r[i]=std::tanh((mid-side)*gain)/norm;
    }
}
}
