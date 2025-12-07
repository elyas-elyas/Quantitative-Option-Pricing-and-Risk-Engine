#ifndef MONTE_CARLO_GREEKS_H
#define MONTE_CARLO_GREEKS_H

#include "MonteCarlo.h"
#include <iostream>

class MonteCarloGreeks {
private:
    MonteCarloPricer pricer_;
    unsigned int seed_; // We need to store the seed to reset it

public:
    // Constructor
    MonteCarloGreeks(int num_sims, unsigned int seed = 42)
        : pricer_(num_sims, seed), seed_(seed) {}

    // Calculate Delta using Central Difference + Common Random Numbers
    double delta(const Option& option, double spot, double rate, double vol, double epsilon = 0.01) {
        // 1. Reset Seed -> Calc UP
        pricer_.setSeed(seed_);
        double p_up = pricer_.price(option, spot + epsilon, rate, vol, true).first;
        
        // 2. Reset Seed -> Calc DOWN (Critical!)
        pricer_.setSeed(seed_);
        double p_down = pricer_.price(option, spot - epsilon, rate, vol, true).first;
        
        return (p_up - p_down) / (2.0 * epsilon);
    }

    // Calculate Gamma
    double gamma(const Option& option, double spot, double rate, double vol, double epsilon = 0.01) {
        pricer_.setSeed(seed_);
        double p_up = pricer_.price(option, spot + epsilon, rate, vol, true).first;
        
        pricer_.setSeed(seed_);
        double p_base = pricer_.price(option, spot, rate, vol, true).first;
        
        pricer_.setSeed(seed_);
        double p_down = pricer_.price(option, spot - epsilon, rate, vol, true).first;
        
        return (p_up - 2.0 * p_base + p_down) / (epsilon * epsilon);
    }

    // Calculate Vega
    double vega(const Option& option, double spot, double rate, double vol, double epsilon = 0.001) {
        pricer_.setSeed(seed_);
        double p_up = pricer_.price(option, spot, rate, vol + epsilon, true).first;
        
        pricer_.setSeed(seed_);
        double p_down = pricer_.price(option, spot, rate, vol - epsilon, true).first;
        
        return (p_up - p_down) / (2.0 * epsilon);
    }
    
    // Calculate Rho
    double rho(const Option& option, double spot, double rate, double vol, double epsilon = 0.001) {
        pricer_.setSeed(seed_);
        double p_up = pricer_.price(option, spot, rate + epsilon, vol, true).first;
        
        pricer_.setSeed(seed_);
        double p_down = pricer_.price(option, spot, rate - epsilon, vol, true).first;
        
        return (p_up - p_down) / (2.0 * epsilon);
    }
    
    void setNumSimulations(int n) {
        pricer_.setNumSimulations(n);
    }
};

#endif // MONTE_CARLO_GREEKS_H