#pragma once
#include <string>
#include <fstream>
#include "Attribute_NFA.hpp"
#include "Attribute_DFA.hpp"
std::unordered_map<std::string, Fast_Attribute_DFA> memopad;
inline Fast_Attribute_DFA compile_regex(std::string_view regex)
{
    Attribute_NFA nfa(std::string(regex.data(), regex.size()));
    Fast_Attribute_DFA dfa = nfa.to_DFA();
    return dfa;
}

inline Fast_Attribute_DFA compile_regex(std::string regex)
{
    return compile_regex(std::string_view(regex.c_str(), regex.size()));
}

inline Fast_Attribute_DFA& compile_regex_memoized(std::string regex)
{
    auto record = memopad.find(regex);
    if(record != memopad.end())
        return record->second;
    Fast_Attribute_DFA dfa = compile_regex(regex);
    memopad.insert(std::make_pair(regex, std::move(dfa)));
    return memopad.find(regex)->second;
}
inline void clear_memopad()
{
    memopad.clear();
}