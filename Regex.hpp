#pragma once
#include <string>
#include <fstream>
#include "Attribute_NFA.hpp"
#include "Attribute_DFA.hpp"
inline Fast_Attribute_DFA compile_regex(std::string regex)
{
    Attribute_NFA nfa(regex);
    Fast_Attribute_DFA dfa = nfa.to_DFA();
    return dfa;
}
inline Fast_Attribute_DFA compile_regex(std::string_view regex)
{
    Attribute_NFA nfa(std::string(regex.data(), regex.size()));
    Fast_Attribute_DFA dfa = nfa.to_DFA();
    return dfa;
}
