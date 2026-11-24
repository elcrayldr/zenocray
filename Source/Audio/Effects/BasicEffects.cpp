#include "BasicEffects.h"
#include <cmath>

namespace cray {
void BasicEffects::prepare(double, int, int) {}

void BasicEffects::process(juce::AudioBuffer<float>& buffer) noexcept {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numSamples <= 0 || numChannels <= 0) return;

    const float saturationMix = drive;
    const float saturationGain = 1.0f + saturationMix * 8.0f;
    const float normalisation = std::tanh(saturationGain);

    if (numChannels == 1) {
        auto* mono = buffer.getWritePointer(0);
        for (int i = 0; i < numSamples; ++i) {
            const float dry = mono[i];
            const float saturated = std::tanh(dry * saturationGain) / normalisation;
            mono[i] = dry + (saturated - dry) * saturationMix;
        }
        return;
    }

    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    for (int i = 0; i < numSamples; ++i) {
        const float mid = (left[i] + right[i]) * 0.5f;
        const float side = (left[i] - right[i]) * 0.5f * width;
        const float widthLeft = mid + side;
        const float widthRight = mid - side;

        const float satLeft = std::tanh(widthLeft * saturationGain) / normalisation;
        const float satRight = std::tanh(widthRight * saturationGain) / normalisation;
        left[i] = widthLeft + (satLeft - widthLeft) * saturationMix;
        right[i] = widthRight + (satRight - widthRight) * saturationMix;
    }
}
}
