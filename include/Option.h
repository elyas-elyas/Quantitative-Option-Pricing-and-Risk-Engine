#ifndef OPTION_H
#define OPTION_H

#include <string>

// Enumeration for option type
enum class OptionType {
    CALL,
    PUT
};

// Abstract base class for all options
class Option {
protected:
    double strike_;           // Strike price (K)
    double maturity_;         // Time to maturity in years (T)
    OptionType type_;         // Type: Call or Put
    
public:
    // Constructor
    Option(double strike, double maturity, OptionType type)
        : strike_(strike), maturity_(maturity), type_(type) {}
    
    // Virtual destructor
    virtual ~Option() = default;
    
    // Pure virtual method for payoff calculation
    virtual double payoff(double spot) const = 0;
    
    // Getters
    double getStrike() const { return strike_; }
    double getMaturity() const { return maturity_; }
    OptionType getType() const { return type_; }
    
    // Convert type to string
    std::string getTypeString() const {
        return (type_ == OptionType::CALL) ? "Call" : "Put";
    }
};

#endif // OPTION_H