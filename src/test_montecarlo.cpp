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
    std::cout << "   Monte Carlo Simulation vs Black-Scholes\n";
    printSeparator();

    // Market Parameters (Same as Test 1)
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double vol = 0.20;
    double maturity = 1.0;
    
    // 1. Analytical Calculation (The Benchmark)
    EuropeanOption callOption(strike, maturity, OptionType::CALL);
    BlackScholes bs(spot, strike, rate, vol, maturity, OptionType::CALL);
    double bs_price = bs.price();

    std::cout << "Option: ATM Call (S=100, K=100, r=5%, vol=20%, T=1)\n";
    std::cout << "BLACK-SCHOLES PRICE (Exact): " << std::fixed << std::setprecision(4) << bs_price << "\n\n";

    // 2. Monte Carlo Simulation with increasing number of paths
    std::vector<int> simulations = {1000, 10000, 100000, 1000000, 5000000};
    
    MonteCarloPricer mcPricer(0); // Initialization

    std::cout << std::setw(15) << "Simulations" 
              << std::setw(15) << "MC Price" 
              << std::setw(15) << "Error" 
              << std::setw(15) << "Std Err" << "\n";
    std::cout << std::string(60, '-') << "\n";

    for (int n : simulations) {
        mcPricer.setNumSimulations(n);
        
        // Get price and standard error
        std::pair<double, double> result = mcPricer.price(callOption, spot, rate, vol);
        double mc_price = result.first;
        double std_err = result.second;
        
        double diff = std::abs(mc_price - bs_price);

        std::cout << std::setw(15) << n 
                  << std::setw(15) << mc_price 
                  << std::setw(15) << diff 
                  << std::setw(15) << std_err << "\n";
    }

    printSeparator();
    std::cout << "Convergence Analysis:\n";
    std::cout << "When N increases by 100x, precision improves by ~10x (1/sqrt(N) law).\n";
    
    return 0;
}