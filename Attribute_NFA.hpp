#pragma once
#include <string>
#include <fstream>
#include <deque>
#include <set>
#include "utils.hpp"
#include "Attribute_DFA.hpp"
class Regex_Parser_Exception: public std::exception {
    std::string exception;
    public:
    const char* invalid_regex_character = nullptr;
    Regex_Parser_Exception(){}
    Regex_Parser_Exception(std::string exception): exception(exception) {}
    Regex_Parser_Exception(const char* invalid_regex_character): invalid_regex_character(invalid_regex_character) {}
    const char* what() const noexcept override 
    {
        return exception.c_str();
    }
    Regex_Parser_Exception& operator=(const Regex_Parser_Exception& except) noexcept  
    {
        this->exception.assign(except.what());
        return *this;
    }
    void set_message(const std::string& exception)
    {
        this->exception = exception;
    }
};
class Regex_Parser_Exception_Invalid_Kleene_Star_Operation: public Regex_Parser_Exception {
    public:
    Regex_Parser_Exception_Invalid_Kleene_Star_Operation(const char* invalid_character):
    Regex_Parser_Exception(invalid_character){}
};
class Regex_Parser_Exception_Invalid_Kleene_Plus_Operation: public Regex_Parser_Exception {
    public:
    Regex_Parser_Exception_Invalid_Kleene_Plus_Operation(const char* invalid_character):
    Regex_Parser_Exception(invalid_character){}
};
class Regex_Parser_Exception_Invalid_Escape_Sequence: public Regex_Parser_Exception {
    public:
    Regex_Parser_Exception_Invalid_Escape_Sequence(const char* invalid_character):
    Regex_Parser_Exception(invalid_character){}
};
class Regex_Parser_Exception_Invalid_Range: public Regex_Parser_Exception {
    public:
    Regex_Parser_Exception_Invalid_Range(const char* invalid_character):
    Regex_Parser_Exception(invalid_character){}
};
std::pair<size_t, std::string> trim_to_line_on_index(const std::string& text, const size_t index, char delimiter = '\n');
std::string show_error_in_string_single(std::string error_message, std::string erroneous_text, size_t index_of_error, char delimiter = '\n');
std::string_view get_attribute(const std::string_view regex, size_t index);
std::string_view next_block(std::string_view regex, char opening = '(', char closing = ')');
class NFA_State {
        public:
        Span<std::vector<std::set<NFA_State*>>, 128> transitions;
        bool accept;
        std::string_view attribute;
        NFA_State(std::vector<std::set<NFA_State*>>& storage, bool accept, std::string_view attribute): 
        transitions(storage, storage.size()), accept(accept), attribute(attribute)
        {
           for(int i = 0; i < transitions.size(); i++)
               storage.push_back(std::set<NFA_State*>());
        }
        void add_transition(char input, NFA_State* new_state) noexcept
        {
           transitions[input].insert(new_state);
        }
        auto transition_iterator(char input) noexcept
        {
           return transitions[input].begin();
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
    };
namespace std {
  template <> struct hash<std::set<NFA_State*>>
  {
    inline size_t operator()(const std::set<NFA_State*> & set) const
    {

        size_t hash = FNV_OFFSET_32;
        for(auto it = set.begin(); it != set.end(); it++)
        {
            hash ^= (size_t) *it;
            hash *= FNV_PRIME_32;
        }
        return hash;
    }
  };
}
inline std::string to_string(std::string_view str)
{
    return std::string(str.data(), str.size());
}
class Attribute_NFA {
    private:
    std::vector<std::set<NFA_State*>> state_storage;
    std::deque<NFA_State> states;
    std::string regex;
    std::deque<std::string> attributes;
    public:
    Attribute_NFA(std::string regex);
    bool accepting_compound_state(const std::set<NFA_State*>& compound_state);
    size_t highest_priority_attribute(const std::set<NFA_State*>& compound_state, std::unordered_map<std::string_view, size_t>& attributes_priority);
    Fast_Attribute_DFA to_DFA();//invalidates this NFA regex, and atrributes
    template <typename T>
    T epsilon_closure(const T& compound_state)
    {
        T closure;
        std::deque<NFA_State*> queue;
        for(auto it = compound_state.begin(); it != compound_state.end(); it++)
        {
            NFA_State& state = **it;
            if(state.transitions[0].size())
            {
                for(auto closure_state = state.transitions[0].begin(); closure_state != state.transitions[0].end(); closure_state++)
                {
                    if(closure.count(*closure_state) == 0)
                    {
                        queue.push_back(*closure_state);
                        closure.insert(*closure_state);
                    }
                }
            }
        }
        while(queue.size())
        {
            NFA_State& state = **queue.begin();
            queue.pop_front();
            if(state.transitions[0].size())
            {
                for(auto closure_state = state.transitions[0].begin(); closure_state != state.transitions[0].end(); closure_state++)
                {
                    if(closure.count(*closure_state) == 0)
                    {
                        queue.push_back(*closure_state);
                        closure.insert(*closure_state);
                    }
                }
            }
        }
        return closure;
    }
    template <typename T>
    T transition_closure(const char symbol, const T& compound_state)
    {
        T closure;
        for(auto it = compound_state.begin(); it != compound_state.end(); it++)
        {
            NFA_State& state = **it;
            if(state.transitions[symbol].size())
            {
                for(auto closure_state = state.transitions[symbol].begin(); closure_state != state.transitions[symbol].end(); closure_state++)
                {
                    if(closure.count(*closure_state) == 0)
                    {
                        closure.insert(*closure_state);
                    }
                }
                closure.merge(epsilon_closure(state.transitions[symbol]));
            }
        }
        return closure;
    }
    std::string expand_plus(std::string_view regex);
    size_t find_state(NFA_State* c);
    void print();
    NFA_State& create_state(std::string_view attribute) noexcept;
    void trim_block(std::string_view& block, char opening = '(') const noexcept;
    bool is_escaped_symbol(const unsigned char c) const noexcept;
    bool is_escapable_symbol(const char c) const noexcept;
    char encode_escape_symbol(const char c) const noexcept;
    char decode_escape_symbol(const char c) const noexcept;
    NFA_State* load_states(std::string_view regex, NFA_State* start, NFA_State* end, std::string_view current_attribute);
};
