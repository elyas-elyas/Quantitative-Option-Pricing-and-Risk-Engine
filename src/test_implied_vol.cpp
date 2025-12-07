#include <iostream>
#include <iomanip>
#include "BlackScholes.h"
#include "ImpliedVolatility.h"

void printSeparator() {
    std::cout << std::string(70, '=') << "\n";
}

int main() {
    printSeparator();
    std::cout << "   Implied Volatility Solver (Newton-Raphson)\n";
    printSeparator();

    // 1. Setup a specific scenario
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double maturity = 1.0;
    double targetVol = 0.30; // We want the solver to find THIS value
    OptionType type = OptionType::CALL;

    // 2. Calculate the 'Market Price' using the target volatility
    // In real life, this price comes from the exchange.
    BlackScholes bs_target(spot, strike, rate, targetVol, maturity, type);
    double marketPrice = bs_target.price();

    std::cout << "Target Parameters:\n";
    std::cout << "  Spot: " << spot << ", Strike: " << strike << "\n";
    std::cout << "  True Volatility: " << (targetVol * 100) << "%\n";
    std::cout << "  Calculated Market Price: $" << marketPrice << "\n\n";

    // 3. Use the Solver to recover the volatility from the price
    // We start with a guess of 0.5 (50%), so the solver has to work to find 0.3
    std::cout << "Starting Newton-Raphson Solver...\n";
    
    double impliedVol = ImpliedVolatility::calculate(
        marketPrice, spot, strike, rate, maturity, type
    );

    // 4. Check results
    std::cout << "\nResults:\n";
    std::cout << "  Implied Volatility Found: " << std::fixed << std::setprecision(6) << (impliedVol * 100) << "%\n";
    std::cout << "  Error: " << std::abs(impliedVol - targetVol) << "\n";

    if (std::abs(impliedVol - targetVol) < 1e-5) {
        std::cout << "\n SUCCESS: Solver recovered the correct volatility!\n";
    } else {
        std::cout << "\n FAILURE: Solver did not converge.\n";
    }
    
    printSeparator();
    return 0;
}