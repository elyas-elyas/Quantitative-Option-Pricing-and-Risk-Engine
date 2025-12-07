#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "BlackScholes.h"
#include "MonteCarlo.h"

void printSeparator() {
    std::cout << std::string(70, '=') << "\n";
}

int main() {
    printSeparator();
    std::cout << "   Variance Reduction Test: Standard vs Antithetic\n";
    printSeparator();

    // Market Parameters
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double vol = 0.20;
    double maturity = 1.0;
    int n_sims = 100000; // 100k simulations
    
    EuropeanOption callOption(strike, maturity, OptionType::CALL);
    
    // 1. Exact Price (Black-Scholes)
    BlackScholes bs(spot, strike, rate, vol, maturity, OptionType::CALL);
    double bs_price = bs.price();
    std::cout << "Target Price (Black-Scholes): " << bs_price << "\n\n";

    // 2. Monte Carlo Setup
    // Important: Use the SAME seed for fair comparison
    MonteCarloPricer mc_standard(n_sims, 42);
    MonteCarloPricer mc_antithetic(n_sims, 42);

    // 3. Run Standard MC
    auto res_std = mc_standard.price(callOption, spot, rate, vol, false); // false = Standard
    
    // 4. Run Antithetic MC
    auto res_anti = mc_antithetic.price(callOption, spot, rate, vol, true); // true = Antithetic

    // 5. Display Results
    std::cout << std::left << std::setw(20) << "Method" 
              << std::setw(15) << "Price" 
              << std::setw(15) << "Diff vs BS" << "\n";
    std::cout << std::string(50, '-') << "\n";

    std::cout << std::left << std::setw(20) << "Standard MC" 
              << std::setw(15) << res_std.first 
              << std::setw(15) << std::abs(res_std.first - bs_price) << "\n";

    std::cout << std::left << std::setw(20) << "Antithetic MC" 
              << std::setw(15) << res_anti.first 
              << std::setw(15) << std::abs(res_anti.first - bs_price) << "\n";
              
    std::cout << "\n";
    
    // Calculate performance improvement
    double improvement = std::abs(res_std.first - bs_price) / std::abs(res_anti.first - bs_price);
    std::cout << "Precision improvement factor: " << std::fixed << std::setprecision(1) << improvement << "x\n";
    
    return 0;
}