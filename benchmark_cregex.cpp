#include <iostream>
#include <regex.h>
#include <chrono>
#include "Regex.hpp"

int main() {
    const std::string input = "ipsum";
    const char* pattern = "ipsum";
    const int iterations = 100000;

    regex_t regex;
    regcomp(&regex, pattern, REG_EXTENDED);
    bool found;
    {
        // Measure runtime of C regex.h
        auto regexStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            found = !regexec(&regex, input.c_str(), 0, NULL, 0);
        }
        auto regexEnd = std::chrono::high_resolution_clock::now();
        auto regexDuration = std::chrono::duration_cast<std::chrono::microseconds>(regexEnd - regexStart).count();
    
        // Print results
        std::cout << "C regex.h: " << iterations << " matches, Total Runtime: " << regexDuration << " microseconds" << std::endl;
    }
    regfree(&regex);

    {
        auto dfa = compile_regex(std::string_view(pattern));
        // Measure runtime of FastAttributeRegex
        auto regexStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            found = dfa.parse_token_no_attribute(input, 0).priority;
        }
        auto regexEnd = std::chrono::high_resolution_clock::now();
        auto regexDuration = std::chrono::duration_cast<std::chrono::microseconds>(regexEnd - regexStart).count();

        // Print results
        std::cout << "Fast Attribute Regex: " << iterations << " matches, Total Runtime: " << regexDuration << " microseconds" << std::endl;
    }

    return 0;
}
