#include <iostream>
#include <regex.h>
#include <regex>
#include <chrono>
#include "Regex.hpp"

void test(const std::string input, const char* pattern)
{
    const int iterations = 100000;
    double timings[3] {0.0};
    std::cout<<"matching input: "<<input<<" with regex: "<<pattern<<'\n';
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
        timings[0] = regexDuration;
    }
    regfree(&regex);
    {
        std::regex regex(pattern);

        // Measure runtime of C++ std::regex
        auto regexStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            found = std::regex_search(input, regex);
        }
        auto regexEnd = std::chrono::high_resolution_clock::now();
        auto regexDuration = std::chrono::duration_cast<std::chrono::microseconds>(regexEnd - regexStart).count();

        // Print results
        std::cout << "C++ std::regex: " << iterations << " matches, Total Runtime: " << regexDuration << " microseconds" << std::endl;
        timings[1] = regexDuration;
    }
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
        timings[2] = regexDuration;
    }
    std::cout<<"FastAttributeRegex ran in:\n";
    std::cout<<(timings[2] / timings[0] * 100)<<" Percent of the time it took regex.h or "<<(timings[0] / timings[2])<<" times faster\n";
    std::cout<<(timings[2] / timings[1] * 100)<<" Percent of the time it took std::regex or "<<(timings[1] / timings[2])<<" times faster\n\n";

}



int main() {

    const std::string input = "1110";
    test(input, "0?1+0+");
    test(input, "0?0?0?0?0?0?0?0?0?0?1111+");
    test("0111111111", "0?1111111*");
    return 0;
}
