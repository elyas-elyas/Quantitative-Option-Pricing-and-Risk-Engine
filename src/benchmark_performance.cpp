#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include "BlackScholes.h"
#include "MonteCarlo.h"

// Helper function to measure time
template<typename Func>
long long measure_execution_time(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void printSeparator() {
    std::cout << std::string(70, '=') << "\n";
}

int main() {
    printSeparator();
    std::cout << "   Performance Benchmark: C++ Pricing Engine\n";
    printSeparator();

    // Parameters
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double vol = 0.20;
    double maturity = 1.0;
    OptionType type = OptionType::CALL;

    // --- TEST 1: BLACK-SCHOLES THROUGHPUT ---
    std::cout << "1. Benchmarking Black-Scholes Analytical Formula...\n";
    
    // We run it 10 million times to get a measurable duration
    const int BS_ITERATIONS = 10'000'000;
    
    // Create the object once to measure pure pricing method speed
    BlackScholes bs(spot, strike, rate, vol, maturity, type);
    
    // Volatile variable to prevent compiler optimization (dead code elimination)
    volatile double dummySum = 0.0;

    auto bs_duration = measure_execution_time([&]() {
        for(int i = 0; i < BS_ITERATIONS; ++i) {
            dummySum = dummySum + bs.price();
        }
    });

    std::cout << "   Iterations: " << BS_ITERATIONS << "\n";
    std::cout << "   Total Time: " << bs_duration << " ms\n";
    double bs_ops = (double)BS_ITERATIONS / (bs_duration / 1000.0);
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << bs_ops << " options/sec\n\n";


    // --- TEST 2: MONTE CARLO SIMULATION SPEED ---
    std::cout << "2. Benchmarking Monte Carlo Simulation...\n";
    
    // We run 1 million paths (simulations) in a single pricing call
    const int MC_PATHS = 1'000'000;
    EuropeanOption option(strike, maturity, type);
    MonteCarloPricer pricer(MC_PATHS);

    auto mc_duration = measure_execution_time([&]() {
        pricer.price(option, spot, rate, vol);
    });

    std::cout << "   Simulations: " << MC_PATHS << "\n";
    std::cout << "   Total Time:  " << mc_duration << " ms\n";
    
    // Calculate paths per second
    double mc_pps = (double)MC_PATHS / (mc_duration / 1000.0);
    std::cout << "   Throughput:  " << std::fixed << std::setprecision(0) << mc_pps << " paths/sec\n";
    
    printSeparator();
    std::cout << "Performance Analysis:\n";
    if (bs_ops > 1'000'000) {
        std::cout << "Black-Scholes is extremely fast (>1M ops/sec).\n";
    }
    std::cout << "ℹMonte Carlo is computationally intensive (Generating random numbers).\n";
    
    return 0;
}