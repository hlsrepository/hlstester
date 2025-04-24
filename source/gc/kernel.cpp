# include <stdexcept>  // supports std::invalid_argument
# include "bubble.h"

// Define global variables to store intermediate results
int add_result;
int subtract_result;
int bitwise_and_result;
int bitwise_or_result;
int bitwise_xor_result;
int mod_result;

int addAndDivide(int a, int b) {
    // Calculate addition and subtraction
    add_result = a + b;
    subtract_result = a - b;
    
    // Calculate the additional four values
    bitwise_and_result = a & b;
    bitwise_or_result  = a | b;
    bitwise_xor_result = a ^ b;
    if (b != 0) {
        mod_result = a % b;
    } else {
        mod_result = 0;  // Or throw an exception depending on requirements
    }
    
    // Prevent division by zero
    if (subtract_result == 0) {
        return subtract_result;
    }
    return add_result / subtract_result;
}

int addAndMultiply(int a, int b) {
    // Calculate addition and subtraction
    add_result = a + b;
    subtract_result = a - b;
    
    // Calculate the additional four values
    bitwise_and_result = a & b;
    bitwise_or_result  = a | b;
    bitwise_xor_result = a ^ b;
    if (b != 0) {
        mod_result = a % b;
    } else {
        mod_result = 0;
    }
    
    return add_result * subtract_result;
}
