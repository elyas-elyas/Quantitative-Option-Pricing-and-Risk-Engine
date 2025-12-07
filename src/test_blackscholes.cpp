#include <iostream>
#include <iomanip>
#include <string>
#include "BlackScholes.h"

void printSeparator() {
    std::cout << std::string(70, '=') << "\n";
}

void printHeader(const std::string& title) {
    printSeparator();
    std::cout << "   " << title << "\n";
    printSeparator();
    std::cout << "\n";
}

void printOptionDetails(const BlackScholes& bs) {
    std::cout << "Market Parameters:\n";
    std::cout << std::string(50, '-') << "\n";
    std::cout << std::left << std::setw(25) << "Spot Price (S):" 
              << "$" << std::fixed << std::setprecision(2) << bs.getSpot() << "\n";
    std::cout << std::setw(25) << "Strike Price (K):" 
              << "$" << bs.getStrike() << "\n";
    std::cout << std::setw(25) << "Risk-free Rate (r):" 
              << std::setprecision(2) << (bs.getRate() * 100) << "%\n";
    std::cout << std::setw(25) << "Volatility (σ):" 
              << std::setprecision(2) << (bs.getVolatility() * 100) << "%\n";
    std::cout << std::setw(25) << "Time to Maturity (T):" 
              << std::setprecision(2) << bs.getMaturity() << " years\n";
    std::cout << std::setw(25) << "Option Type:" 
              << (bs.getType() == OptionType::CALL ? "Call" : "Put") << "\n";
    std::cout << "\n";
}

void printGreeks(const BlackScholes& bs) {
    std::cout << "Option Price and Greeks:\n";
    std::cout << std::string(50, '-') << "\n";
    
    double price = bs.price();
    double delta = bs.delta();
    double gamma = bs.gamma();
    double vega = bs.vega();
    double theta = bs.theta();
    double rho = bs.rho();
    
    std::cout << std::left << std::setw(20) << "Price:" 
              << "$" << std::fixed << std::setprecision(4) << price << "\n";
    std::cout << std::setw(20) << "Delta (Δ):" 
              << std::setprecision(6) << delta << "\n";
    std::cout << std::setw(20) << "Gamma (Γ):" 
              << std::setprecision(6) << gamma << "\n";
    std::cout << std::setw(20) << "Vega (ν):" 
              << std::setprecision(6) << vega << "\n";
    std::cout << std::setw(20) << "Theta (Θ):" 
              << std::setprecision(6) << theta << "\n";
    std::cout << std::setw(20) << "Rho (ρ):" 
              << std::setprecision(6) << rho << "\n";
    std::cout << "\n";
}

void testScenario(const std::string& scenarioName, double spot, double strike, 
                  double rate, double vol, double maturity, OptionType type) {
    std::cout << "\n📊 " << scenarioName << "\n\n";
    
    BlackScholes bs(spot, strike, rate, vol, maturity, type);
    printOptionDetails(bs);
    printGreeks(bs);
}

int main() {
    printHeader("Black-Scholes Option Pricing Engine");
    
    // Test 1: At-the-Money Call
    testScenario("Test 1: At-the-Money Call Option",
                 100.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.20,   // volatility (20%)
                 1.0,    // maturity (1 year)
                 OptionType::CALL);
    
    // Test 2: At-the-Money Put
    testScenario("Test 2: At-the-Money Put Option",
                 100.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.20,   // volatility (20%)
                 1.0,    // maturity (1 year)
                 OptionType::PUT);
    
    // Test 3: In-the-Money Call
    testScenario("Test 3: In-the-Money Call Option",
                 110.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.20,   // volatility (20%)
                 1.0,    // maturity (1 year)
                 OptionType::CALL);
    
    // Test 4: Out-of-the-Money Put
    testScenario("Test 4: Out-of-the-Money Put Option",
                 110.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.20,   // volatility (20%)
                 1.0,    // maturity (1 year)
                 OptionType::PUT);
    
    // Test 5: High Volatility Impact
    testScenario("Test 5: High Volatility Call (40%)",
                 100.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.40,   // volatility (40%)
                 1.0,    // maturity (1 year)
                 OptionType::CALL);
    
    // Test 6: Short Maturity
    testScenario("Test 6: Short Maturity Call (3 months)",
                 100.0,  // spot
                 100.0,  // strike
                 0.05,   // rate (5%)
                 0.20,   // volatility (20%)
                 0.25,   // maturity (3 months)
                 OptionType::CALL);
    
    printSeparator();
    std::cout << "✅ Black-Scholes pricing tests complete!\n";
    std::cout << "Next step: Implement Monte Carlo simulation\n";
    printSeparator();
    
    return 0;
}