#include <iostream>
#include "Fact.hpp"


int main() {
    try {
        int number;
        std::cout << "Enter natural number: ";
        FactNumInput(number);
        
        auto factorials = FindFactorials(number);
        
        std::cout << "\nFactorials modulo " << MOD << ":\n";
        for (size_t i = 0; i < factorials.size(); ++i) {
            std::cout << i << "! = " << factorials[i] << '\n';
        }
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Input error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    }
    
    return 0;
}