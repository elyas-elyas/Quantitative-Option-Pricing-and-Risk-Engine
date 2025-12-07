#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include "EuropeanOption.h"
#include "Utils.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <omp.h> // Header pour OpenMP

class MonteCarloPricer {
private:
    int num_sims_;
    unsigned int seed_; // On stocke la graine de base pour la reproduction

public:
    // Constructeur
    MonteCarloPricer(int num_sims, unsigned int seed = 42)
        : num_sims_(num_sims), seed_(seed) {}

    // Permet de changer la seed (utile pour les calculs de Greeks)
    void setSeed(unsigned int seed) { seed_ = seed; }

    // Méthode principale de pricing (Multithreadée)
    std::pair<double, double> price(const Option& option, 
                                    double spot, 
                                    double rate, 
                                    double volatility, 
                                    bool use_antithetic = true) {
        
        double T = option.getMaturity();
        double drift = (rate - 0.5 * volatility * volatility) * T;
        double diffusion = volatility * std::sqrt(T);
        double discount_factor = std::exp(-rate * T);
        
        double sum_payoffs = 0.0;
        double sum_sq_payoffs = 0.0;

        // Nombre de boucles : si antithetic, on fait moitié moins d'itérations (mais 2 calculs par itération)
        int loops = use_antithetic ? (num_sims_ / 2) : num_sims_;
        int actual_sims = use_antithetic ? (loops * 2) : num_sims_;

        // --- DÉBUT DE la ZONE PARALLÈLE ---
        // reduction(+:...) permet d'additionner les résultats de chaque thread à la fin sans conflit
        #pragma omp parallel reduction(+:sum_payoffs, sum_sq_payoffs)
        {
            // CRITIQUE : Chaque thread doit avoir son propre générateur aléatoire
            // Sinon, ils accèdent tous au même rng_ et créent des conflits (Data Race)
            int thread_id = omp_get_thread_num();
            RandomGenerator local_rng(seed_ + thread_id + 1); 

            // Distribue les itérations de la boucle 'i' entre les différents threads disponibles
            #pragma omp for
            for (int i = 0; i < loops; ++i) {
                double Z = local_rng.getNormal();
                
                // Chemin 1
                double S_T1 = spot * std::exp(drift + diffusion * Z);
                double payoff1 = option.payoff(S_T1);

                if (use_antithetic) {
                    // Chemin 2 (Antithétique)
                    double S_T2 = spot * std::exp(drift + diffusion * (-Z));
                    double payoff2 = option.payoff(S_T2);
                    
                    sum_payoffs += payoff1 + payoff2;
                    sum_sq_payoffs += payoff1 * payoff1 + payoff2 * payoff2;
                } else {
                    sum_payoffs += payoff1;
                    sum_sq_payoffs += payoff1 * payoff1;
                }
            }
        } 
        // --- FIN DE LA ZONE PARALLÈLE ---

        // Moyenne et actualisation
        double mean_payoff = sum_payoffs / actual_sims;
        double estimated_price = mean_payoff * discount_factor;

        // Variance et erreur standard
        double variance = (sum_sq_payoffs / actual_sims) - (mean_payoff * mean_payoff);
        // Sécurité numérique : la variance ne peut pas être négative
        if (variance < 0) variance = 0.0;
        
        double std_error = (std::sqrt(variance) / std::sqrt(actual_sims)) * discount_factor;

        return {estimated_price, std_error};
    }
    
    void setNumSimulations(int n) { num_sims_ = n; }
};

#endif // MONTE_CARLO_H