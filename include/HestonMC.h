#ifndef HESTON_MC_H
#define HESTON_MC_H

#include "Option.h"
#include "Utils.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <omp.h>

class HestonPricer {
private:
    int num_sims_;
    int num_steps_; // Number of time steps (e.g., 252 for daily simulations)

public:
    // Constructor
    HestonPricer(int num_sims, int num_steps = 100)
        : num_sims_(num_sims), num_steps_(num_steps) {}

    // Heston Monte Carlo Pricing Method
    double price(const Option& option, 
                 double spot, 
                 double rate, 
                 double v0,     // Initial Variance (volatility^2)
                 double kappa,  // Mean Reversion Speed
                 double theta,  // Long-run Variance
                 double xi,     // Volatility of Volatility (Vol-of-Vol)
                 double rho) {  // Correlation between Spot and Volatility
        
        double T = option.getMaturity();
        double dt = T / num_steps_;
        double discount_factor = std::exp(-rate * T);
        
        double sum_payoffs = 0.0;

        // Pre-calculate correlation constants
        // We generate two independent standard normals Z1, Z2
        // The correlated Brownian motion for Vol is: Wv = rho*Z1 + sqrt(1-rho^2)*Z2
        double sqrt_dt = std::sqrt(dt);
        double c1 = rho;
        double c2 = std::sqrt(1.0 - rho * rho);

        // --- PARALLEL REGION (OpenMP) ---
        #pragma omp parallel reduction(+:sum_payoffs)
        {
            int thread_id = omp_get_thread_num();
            RandomGenerator rng(42 + thread_id); // Unique seed per thread

            #pragma omp for
            for (int i = 0; i < num_sims_; ++i) {
                double S = spot;
                double v = v0;

                // Time-Stepping Simulation (Euler-Maruyama with Full Truncation)
                for (int t = 0; t < num_steps_; ++t) {
                    double Z1 = rng.getNormal();
                    double Z2 = rng.getNormal();

                    // Correlate Brownian motions
                    double dWs = Z1 * sqrt_dt;                  // Asset noise
                    double dWv = (c1 * Z1 + c2 * Z2) * sqrt_dt; // Volatility noise

                    // 1. Update Volatility (CIR Process)
                    // Use "Full Truncation" scheme to prevent negative variance
                    double v_curr = std::max(v, 0.0);
                    double dv = kappa * (theta - v_curr) * dt + xi * std::sqrt(v_curr) * dWv;
                    v += dv;

                    // 2. Update Asset Price
                    // S(t+1) = S(t) * exp( (r - 0.5*v)*dt + sqrt(v)*dWs )
                    double drift = (rate - 0.5 * v_curr) * dt;
                    double diffusion = std::sqrt(v_curr) * dWs;
                    S *= std::exp(drift + diffusion);
                }

                sum_payoffs += option.payoff(S);
            }
        }
        
        return (sum_payoffs / num_sims_) * discount_factor;
    }
};

#endif // HESTON_MC_H