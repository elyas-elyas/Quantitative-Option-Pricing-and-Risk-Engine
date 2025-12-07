#include <iostream>
#include <iomanip>
#include "EuropeanOption.h"
#include "Utils.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "   Option Pricing Engine - Test v0.1\n";
    std::cout << "========================================\n\n";
    
    // Example parameters
    double strike = 100.0;
    double maturity = 1.0;  // 1 year
    
    // Create a European Call option
    EuropeanOption callOption(strike, maturity, OptionType::CALL);
    
    std::cout << "Option Type: " << callOption.getTypeString() << "\n";
    std::cout << "Strike: $" << callOption.getStrike() << "\n";
    std::cout << "Maturity: " << callOption.getMaturity() << " years\n\n";
    
    // Test payoff for different spot prices
    std::cout << "Payoff Calculation:\n";
    std::cout << std::setw(15) << "Spot Price" 
              << std::setw(15) << "Payoff\n";
    std::cout << std::string(30, '-') << "\n";
    
    for (double spot = 80.0; spot <= 120.0; spot += 10.0) {
        double payoff = callOption.payoff(spot);
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) 
                  << spot << std::setw(15) << payoff << "\n";
    }
    
    std::cout << "\n";
    
    // Test a Put option
    EuropeanOption putOption(strike, maturity, OptionType::PUT);
    
    std::cout << "\nOption Type: " << putOption.getTypeString() << "\n";
    std::cout << "Strike: $" << putOption.getStrike() << "\n";
    std::cout << "Maturity: " << putOption.getMaturity() << " years\n\n";
    
    std::cout << "Payoff Calculation:\n";
    std::cout << std::setw(15) << "Spot Price" 
              << std::setw(15) << "Payoff\n";
    std::cout << std::string(30, '-') << "\n";
    
    for (double spot = 80.0; spot <= 120.0; spot += 10.0) {
        double payoff = putOption.payoff(spot);
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) 
                  << spot << std::setw(15) << payoff << "\n";
    }
    
    // Test utility functions
    std::cout << "\n\nUtility Functions Test:\n";
    std::cout << std::string(30, '-') << "\n";
    std::cout << "N(0) = " << normalCDF(0.0) << " (should be ~0.5)\n";
    std::cout << "N(1) = " << normalCDF(1.0) << " (should be ~0.84)\n";
    std::cout << "N(-1) = " << normalCDF(-1.0) << " (should be ~0.16)\n";
    
    std::cout << "\nRandom Normal Numbers (first 10):\n";
    RandomGenerator rng(42);
    for (int i = 0; i < 10; ++i) {
        std::cout << "  " << std::setprecision(4) << rng.getNormal() << "\n";
    }
    
    std::cout << "\n Basic setup complete!\n";
    std::cout << "Next step: Implement Black-Scholes pricing\n\n";
    
    return 0;
}