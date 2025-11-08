#ifndef FACT_HPP
#define FACT_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <limits>


int MOD = 1000000007;

void FactNumInput(int& n) {
    if (!(std::cin >> n)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("Invalid input: must be a natural number");
    }
    
    if (n < 0) {
        throw std::invalid_argument("Invalid input: negative number");
    }
}

std::vector<int> FindFactorials(int n) {
    std::vector<int> factorials;
    factorials.reserve(static_cast<size_t>(n) + 1);
    
    int fact = 1;
    factorials.push_back(fact);
    
    for (int i = 1; i <= n; ++i) {
        fact = static_cast<int>((static_cast<unsigned long long>(fact) * i) % MOD);
        factorials.push_back(fact);
    }
    return factorials;
}


#endif FACT_HPP