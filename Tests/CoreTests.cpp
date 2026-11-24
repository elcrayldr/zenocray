#include <cmath>
#include <iostream>
int main() {
    constexpr double sampleRate = 48000.0, frequency = 440.0;
    const double increment = frequency / sampleRate;
    if (!(increment > 0.0 && increment < 0.5)) return 1;
    double phase = 0.0;
    for (int i = 0; i < 48000; ++i) { phase += increment; if (phase >= 1.0) phase -= std::floor(phase); }
    if (std::abs(phase) > 1.0e-8) return 2;
    for (int i = -10000; i <= 10000; ++i) {
        const double y = std::tanh(static_cast<double>(i) / 1000.0);
        if (!std::isfinite(y) || std::abs(y) > 1.0) return 3;
    }
    std::cout << "Urban core math sanity tests passed\n";
    return 0;
}
