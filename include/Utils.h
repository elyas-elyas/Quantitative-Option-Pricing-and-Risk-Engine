#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <random>

// Constants
const double PI = 3.14159265358979323846;

// Standard normal probability density function
inline double normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * PI)) * std::exp(-0.5 * x * x);
}

// Standard normal cumulative distribution function (approximation)
inline double normalCDF(double x) {
    // Abramowitz and Stegun approximation
    const double a1 =  0.254829592;
    const double a2 = -0.284496736;
    const double a3 =  1.421413741;
    const double a4 = -1.453152027;
    const double a5 =  1.061405429;
    const double p  =  0.3275911;
    
    int sign = 1;
    if (x < 0) {
        sign = -1;
    }
    x = std::abs(x) / std::sqrt(2.0);
    
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);
    
    return 0.5 * (1.0 + sign * y);
}

// Normal random number generator (Box-Muller)
class RandomGenerator {
private:
    std::mt19937 generator_;
    std::normal_distribution<double> distribution_;
    
public:
    RandomGenerator(unsigned int seed = 42) 
        : generator_(seed), distribution_(0.0, 1.0) {}
    
    double getNormal() {
        return distribution_(generator_);
    }
    
    void setSeed(unsigned int seed) {
        generator_.seed(seed);
    }
};

#endif // UTILS_H