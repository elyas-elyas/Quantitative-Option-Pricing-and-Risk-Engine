#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "BlackScholes.h"
#include "MonteCarloGreeks.h"

void printSeparator() {
    std::cout << std::string(70, '=') << "\n";
}

int main() {
    printSeparator();
    std::cout << "   Greeks Calculation: Analytical vs Monte Carlo\n";
    printSeparator();

    // Market Parameters
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double vol = 0.20;
    double maturity = 1.0;
    int n_sims = 500000; // High number for stable Gamma
    
    EuropeanOption callOption(strike, maturity, OptionType::CALL);
    
    // 1. Analytical Greeks (Black-Scholes)
    BlackScholes bs(spot, strike, rate, vol, maturity, OptionType::CALL);
    
    double bs_delta = bs.delta();
    double bs_gamma = bs.gamma();
    double bs_vega = bs.vega();
    double bs_rho = bs.rho();

    // 2. Monte Carlo Greeks (Finite Differences)
    // We use the same seed implicitly inside the class for consistent bumping
    MonteCarloGreeks mcGreeks(n_sims, 42); 

    std::cout << "Calculating MC Greeks with " << n_sims << " simulations...\n";
    double mc_delta = mcGreeks.delta(callOption, spot, rate, vol);
    double mc_gamma = mcGreeks.gamma(callOption, spot, rate, vol);
    double mc_vega = mcGreeks.vega(callOption, spot, rate, vol);
    double mc_rho = mcGreeks.rho(callOption, spot, rate, vol);

    // 3. Display Comparison
    std::cout << "\nComparison (Spot=" << spot << "):\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << std::left << std::setw(15) << "Greek" 
              << std::setw(15) << "Analytical" 
              << std::setw(15) << "Monte Carlo" 
              << std::setw(15) << "Error" << "\n";
    std::cout << std::string(60, '-') << "\n";

    auto printRow = [](std::string name, double exact, double approx) {
        std::cout << std::left << std::setw(15) << name 
                  << std::setw(15) << std::fixed << std::setprecision(5) << exact 
                  << std::setw(15) << approx 
                  << std::setw(15) << std::abs(exact - approx) << "\n";
    };

    printRow("Delta", bs_delta, mc_delta);
    printRow("Gamma", bs_gamma, mc_gamma);
    printRow("Vega", bs_vega, mc_vega);
    printRow("Rho", bs_rho, mc_rho);

    std::cout << "\nNote: Gamma is the hardest to estimate via MC (second derivative).\n";
    printSeparator();
    
    return 0;
}