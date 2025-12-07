#ifndef IMPLIED_VOLATILITY_H
#define IMPLIED_VOLATILITY_H

#include "BlackScholes.h"
#include <cmath>
#include <iostream>

class ImpliedVolatility {
public:
    // Newton-Raphson method to find Implied Volatility
    static double calculate(double marketPrice, 
                            double spot, 
                            double strike, 
                            double rate, 
                            double maturity, 
                            OptionType type,
                            double initialGuess = 0.5,
                            double epsilon = 1e-6,
                            int maxIterations = 100) {
        
        double sigma = initialGuess; // Start with a guess (e.g., 50%)

        for (int i = 0; i < maxIterations; ++i) {
            // 1. Calculate Price and Vega with current sigma
            BlackScholes bs(spot, strike, rate, sigma, maturity, type);
            double price = bs.price();
            double vega = bs.vega();

            // 2. Calculate the difference (f(x))
            double diff = price - marketPrice;

            // 3. Check for convergence (are we close enough?)
            if (std::abs(diff) < epsilon) {
                return sigma;
            }

            // 4. Safety check: avoid division by zero if Vega is too small
            // (Happens for deep ITM/OTM options)
            if (std::abs(vega) < 1e-8) {
                std::cerr << "Warning: Vega is too small, solver might fail.\n";
                break;
            }

            // 5. Newton-Raphson Step: x_new = x - f(x) / f'(x)
            sigma = sigma - (diff / vega);
        }

        return -1.0; // Return -1 if failed to converge
    }
};

#endif // IMPLIED_VOLATILITY_H