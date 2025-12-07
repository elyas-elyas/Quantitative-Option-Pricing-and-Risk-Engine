#ifndef EUROPEAN_OPTION_H
#define EUROPEAN_OPTION_H

#include "Option.h"
#include <algorithm>

// European option (exercise only at maturity)
class EuropeanOption : public Option {
public:
    // Constructor
    EuropeanOption(double strike, double maturity, OptionType type)
        : Option(strike, maturity, type) {}
    
    // Calculate payoff at maturity
    double payoff(double spot) const override {
        if (type_ == OptionType::CALL) {
            // Call: max(S - K, 0)
            return std::max(spot - strike_, 0.0);
        } else {
            // Put: max(K - S, 0)
            return std::max(strike_ - spot, 0.0);
        }
    }
};

#endif // EUROPEAN_OPTION_H