#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

#include "EuropeanOption.h"
#include "Utils.h"
#include <cmath>

class BlackScholes {
private:
    double spot_;          // Current stock price (S)
    double strike_;        // Strike price (K)
    double rate_;          // Risk-free interest rate (r)
    double volatility_;    // Volatility (sigma)
    double maturity_;      // Time to maturity (T)
    OptionType type_;      // Call or Put
    
    // Calculate d1 parameter
    double calculateD1() const {
        return (std::log(spot_ / strike_) + (rate_ + 0.5 * volatility_ * volatility_) * maturity_) 
               / (volatility_ * std::sqrt(maturity_));
    }
    
    // Calculate d2 parameter
    double calculateD2() const {
        return calculateD1() - volatility_ * std::sqrt(maturity_);
    }
    
public:
    // Constructor
    BlackScholes(double spot, double strike, double rate, double volatility, 
                 double maturity, OptionType type)
        : spot_(spot), strike_(strike), rate_(rate), 
          volatility_(volatility), maturity_(maturity), type_(type) {}
    
    // Calculate option price using Black-Scholes formula
    double price() const {
        double d1 = calculateD1();
        double d2 = calculateD2();
        
        double discount_factor = std::exp(-rate_ * maturity_);
        
        if (type_ == OptionType::CALL) {
            // Call price: S*N(d1) - K*e^(-rT)*N(d2)
            return spot_ * normalCDF(d1) - strike_ * discount_factor * normalCDF(d2);
        } else {
            // Put price: K*e^(-rT)*N(-d2) - S*N(-d1)
            return strike_ * discount_factor * normalCDF(-d2) - spot_ * normalCDF(-d1);
        }
    }
    
    // Calculate Delta (∂V/∂S)
    double delta() const {
        double d1 = calculateD1();
        
        if (type_ == OptionType::CALL) {
            return normalCDF(d1);
        } else {
            return normalCDF(d1) - 1.0;
        }
    }
    
    // Calculate Gamma (∂²V/∂S²)
    double gamma() const {
        double d1 = calculateD1();
        return normalPDF(d1) / (spot_ * volatility_ * std::sqrt(maturity_));
    }
    
    // Calculate Vega (∂V/∂σ)
    double vega() const {
        double d1 = calculateD1();
        return spot_ * normalPDF(d1) * std::sqrt(maturity_);
    }
    
    // Calculate Theta (∂V/∂t)
    double theta() const {
        double d1 = calculateD1();
        double d2 = calculateD2();
        
        double term1 = -(spot_ * normalPDF(d1) * volatility_) / (2.0 * std::sqrt(maturity_));
        
        if (type_ == OptionType::CALL) {
            double term2 = rate_ * strike_ * std::exp(-rate_ * maturity_) * normalCDF(d2);
            return term1 - term2;
        } else {
            double term2 = rate_ * strike_ * std::exp(-rate_ * maturity_) * normalCDF(-d2);
            return term1 + term2;
        }
    }
    
    // Calculate Rho (∂V/∂r)
    double rho() const {
        double d2 = calculateD2();
        
        if (type_ == OptionType::CALL) {
            return strike_ * maturity_ * std::exp(-rate_ * maturity_) * normalCDF(d2);
        } else {
            return -strike_ * maturity_ * std::exp(-rate_ * maturity_) * normalCDF(-d2);
        }
    }
    
    // Getters
    double getSpot() const { return spot_; }
    double getStrike() const { return strike_; }
    double getRate() const { return rate_; }
    double getVolatility() const { return volatility_; }
    double getMaturity() const { return maturity_; }
    OptionType getType() const { return type_; }
};

#endif // BLACK_SCHOLES_H