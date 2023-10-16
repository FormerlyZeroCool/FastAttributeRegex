#pragma once
#include <set>
#include <vector>
#include <array>
#include <list>
#include <unordered_map>
#include <deque>
#include <string>
#include <iostream>
#include <utility>
#include <string>
#include <fstream>
#include "utils.hpp"

struct Fast_Attribute_DFA {
    struct DFA_State {
        std::array<uint32_t, 128> transitions;
        size_t attribute_priority = 0; 
        bool accept;
        DFA_State(const bool accept, const std::string_view attribute, const size_t attribute_priority):
            attribute_priority(attribute_priority), accept(accept)
        {
            for(size_t i = 0; i < transitions.size(); i++)
            {
                transitions[i] = -1;
            }
        }
        void set_transition(char input, uint32_t new_state) noexcept
        {
           transitions[input] = new_state;
        }
        uint32_t transition_iterator(char input) const noexcept
        {
           return transitions[input];
        }
        bool is_accepting() const noexcept
        {
           return accept;
        }
        void set_accepting(bool is_accepting) noexcept
        {
           accept = is_accepting;
        }
        bool operator==(const DFA_State& o) const noexcept
        {
            return this->accept == o.accept && this->attribute_priority == o.attribute_priority && this->transitions == o.transitions;
        }
        static inline const uint32_t rle_key = -2, end_state_encoding = -3;
        //runtime length encode state transitions
        std::ostream& write(std::ostream& o);
        std::istream& read(std::istream& in);
        };
    std::ostream& write(std::ostream& o);
    std::istream& read_string(std::istream& in, std::string& str);
    std::istream& read(std::istream& in);
    void load_file(const std::string filepath);
    void write_to_file(const std::string filepath);
    std::vector<DFA_State> states;
    std::string regex;
    std::vector<std::string> attributes;
    Fast_Attribute_DFA(const std::string& regex, const std::deque<std::string>& attributes);
    bool operator==(const Fast_Attribute_DFA& o) const noexcept;
    friend std::ostream& operator<<(std::ostream& o, const Fast_Attribute_DFA& dfa);
    size_t start_state() const noexcept;
    uint32_t transition(char symbol, uint32_t current_state) const noexcept;
    const std::vector<DFA_State>& get_states() const noexcept;
    const std::string& get_attribute(const DFA_State& state) const noexcept;
    struct ParseRecord {
        const size_t state, end_index;
        ParseRecord(const size_t state, const size_t end): state(state), end_index(end) {}
    };
    ParseRecord parse_token_simple(const std::string_view input_text, const size_t start);
    SimpleLexToken parse_token_no_attribute(const std::string_view input_text, const size_t start);
    LexToken parse_token(const std::string_view input_text, const size_t start);
};
class Attribute_DFA {
    friend class Attribute_NFA;
    public:
    struct DFA_State {
        Span<std::vector<size_t>, 128> transitions; 
        std::string_view attribute;
        size_t attribute_priority = 0;  
        bool accept;
        DFA_State(std::vector<size_t>& storage, const bool accept, const std::string_view attribute, const size_t attribute_priority):
        transitions(storage, storage.size()), attribute(attribute), attribute_priority(attribute_priority), accept(accept)
        {
            for(size_t i = 0; i < transitions.size(); i++)
            {
                storage.push_back(-1);
            }
        }
        void set_transition(char input, size_t new_state) noexcept
        {
           transitions[input] = new_state;
        }
        size_t transition_iterator(char input) const noexcept
        {
           return transitions.get(input);
        }
        bool is_accepting() const noexcept
        {
           return accept;
        }
        void set_accepting(bool is_accepting) noexcept
        {
           accept = is_accepting;
        }
        void set_attribute(std::string_view attribute) noexcept
        {
           this->attribute = attribute;
        }
        size_t hash() const noexcept
        {
            size_t hash = FNV_OFFSET_32;
            for(size_t i = 0; i < transitions.size(); i++)
            {
                if(transitions.get(i) != -1)
                {
                    hash ^= i;
                    hash *= FNV_PRIME_32;
                    hash += (size_t) transitions.get(i);
                }
            }
            return hash;
        }
        bool operator==(const DFA_State& other) const noexcept
        {
            bool equivalent = (this->attribute_priority == other.attribute_priority) & (this->accept == other.accept);
            for(size_t i = 0; equivalent & (i < transitions.size()); i++)
            {
                equivalent = this->transitions.get(i) == other.transitions.get(i);
            }
            return equivalent;
        }
    };
    private:
    std::vector<size_t> state_storage;
    std::deque<DFA_State> states;
    std::string regex;
    std::deque<std::string> attributes;
    std::unordered_map<std::string_view, size_t> attributes_priority;
    public:
    Attribute_DFA(std::string&& regex, std::deque<std::string>&& attributes);
    size_t get_priority(std::string_view attribute);
    std::deque<DFA_State>& get_states() noexcept;
    LexToken parse_token(const std::string_view input_text, const size_t start);
    void set_regex(const std::string& regex) noexcept;
    void print();
    size_t start_state() const noexcept;
    size_t transition(char symbol, size_t current_state) const noexcept;
    Attribute_DFA minimize();
    Attribute_DFA remove_indistinguishible_states();
    //return vector of hashes, and values 
    std::vector<std::pair<size_t, std::list<size_t> > > group_states_by_distiguishibility();
    Fast_Attribute_DFA to_fast_dfa() const noexcept;
};
