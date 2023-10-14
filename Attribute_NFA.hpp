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
    Attribute_NFA(std::string regex): regex(regex)
    {
        attributes.push_back("");
        try {
            this->regex = expand_plus(regex);
            auto& initial_start_state = create_state("");
            size_t first_named_symbol = this->regex.find('@');
            for(size_t i = first_named_symbol; i < this->regex.size();)
            {
                const size_t possible_end = this->regex.find('@', i + 1);
                const size_t end_index = possible_end < this->regex.size() ? possible_end : this->regex.size();
                auto& start_state = create_state("");
                initial_start_state.add_transition(0, &start_state);
                auto& end_state = create_state("");
                end_state.set_accepting(true);
                //parse current attribute name
                std::string_view current_attribute = get_attribute(this->regex, i);
                i += 2 + current_attribute.size();
                this->attributes.push_back(to_string(current_attribute));
                end_state.set_attribute(current_attribute);
                
                load_states(std::string_view(this->regex.c_str() + i, end_index - i), &start_state, &end_state, current_attribute);
                i = end_index;
            }
            if(first_named_symbol == -1)
            {
                auto& start_state = create_state("");
                initial_start_state.add_transition(0, &start_state);
                auto& end_state = create_state("");
                end_state.set_accepting(true);
                load_states(std::string_view(this->regex.c_str(), regex.size()), &start_state, &end_state, "");
            }
        }
        catch(const Regex_Parser_Exception_Invalid_Range& exception)
        {
            throw Regex_Parser_Exception(
                show_error_in_string_single("Error parsing regex invalid range block:", 
                    std::string(this->regex.data(), this->regex.size()), 
                        exception.invalid_regex_character - this->regex.data(), '@'));
        }
        catch(const Regex_Parser_Exception_Invalid_Escape_Sequence& exception)
        {
            if(exception.invalid_regex_character[0] == '\\')
                throw Regex_Parser_Exception(
                    show_error_in_string_single("Error parsing regex invalid use of \\ should be used for escaping try: \\\\", 
                        std::string(regex.data(), regex.size()), 
                            exception.invalid_regex_character - regex.data()));
            else
                throw Regex_Parser_Exception(
                    show_error_in_string_single("Error parsing regex invalid escape sequence:",
                         std::string(regex.data(), regex.size()), 
                            exception.invalid_regex_character - regex.data()));
        }
        catch(const Regex_Parser_Exception_Invalid_Kleene_Plus_Operation& exception)
        {
            throw Regex_Parser_Exception(
                show_error_in_string_single("Error parsing regex invalid use of Kleene plus operation:",
                    std::string(regex.data(), regex.size()),
                        exception.invalid_regex_character - regex.data()));
        }
        catch(const Regex_Parser_Exception_Invalid_Kleene_Star_Operation& exception)
        {
            throw Regex_Parser_Exception(
                show_error_in_string_single("Error parsing regex invalid use of Kleene star operation:",
                    std::string(regex.data(), regex.size()),
                        exception.invalid_regex_character - regex.data()));
        }

    }
    bool accepting_compound_state(const std::set<NFA_State*>& compound_state)
    {
        for(auto it = compound_state.begin(); it != compound_state.end(); it++)
        {
            if((*it)->accept)
                return true;
        }
        return false;
    }
    size_t highest_priority_attribute(const std::set<NFA_State*>& compound_state, std::unordered_map<std::string_view, size_t>& attributes_priority)
    {
        size_t attribute_index = -1;

        for(auto it = compound_state.begin(); it != compound_state.end(); it++)
        {
            if((*it)->accept && attribute_index > attributes_priority[((*it)->attribute)])
            {
                attribute_index = attributes_priority[((*it)->attribute)];
            }
        }
        return attribute_index == -1 ? 0 : attribute_index;
    } 
    Fast_Attribute_DFA to_DFA()//invalidates this NFA regex, and atrributes
    {
        std::unordered_map<std::string_view, size_t> attributes_priority;
        for(size_t i = 0; i < attributes.size(); i++)
        {
            attributes_priority[attributes[i]] = i;
        }
        Attribute_DFA dfa(std::move(regex), std::move(attributes));
        std::vector<std::set<NFA_State*> > compound_states;
        std::deque<Attribute_DFA::DFA_State>& dfa_states = dfa.get_states();
        //for each nfa state create compound states based on epsilon closure, and their transitions
        std::unordered_map<std::set<NFA_State*>, size_t > compound_state_index_map;
        std::set<NFA_State*> state{&states[0]};
        auto closure = epsilon_closure(state);
        compound_states.push_back(closure);
        const size_t priority = highest_priority_attribute(compound_states.back(), attributes_priority);
        dfa_states.push_back(Attribute_DFA::DFA_State(dfa.state_storage, 
                accepting_compound_state(compound_states.back()), dfa.attributes[priority], priority)
            );
        compound_state_index_map[closure] = dfa_states.size() - 1;
        for(size_t state_index = 0; state_index < compound_states.size(); state_index++)
        {

            for(size_t i = 1; i < 128; i++)
            {
                std::set<NFA_State*> closure = transition_closure(i, compound_states[state_index]);
                if(closure.size() == 0)
                {}
                else if(compound_state_index_map.count(closure) == 0)//state does not exist in dfa/compound_states
                {
                    compound_states.push_back(closure);
                    const size_t priority = highest_priority_attribute(compound_states.back(), attributes_priority);
                    dfa_states.push_back(Attribute_DFA::DFA_State(dfa.state_storage, 
                            accepting_compound_state(compound_states.back()), dfa.attributes[priority], priority)
                        );
                    compound_state_index_map[closure] = dfa_states.size() - 1;
                    dfa_states[state_index].set_transition(i, dfa_states.size() - 1);
                }
                else
                {
                    size_t compound_state_index = compound_state_index_map[closure];
                    dfa_states[state_index].set_transition(i, (dfa_states.begin() + compound_state_index) - dfa_states.begin());
                }
            }
        }
        
        return dfa.minimize().to_fast_dfa();
    }
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
    std::string expand_plus(std::string_view regex)
    {
        size_t index = 0;
        std::string result = "";
        while(index < regex.size())
        {
            try{
                std::string_view block = next_block(regex.substr(index));
                if(block[0] == '\\')
                {
                    index++;
                    if(index >= regex.size())
                        throw Regex_Parser_Exception_Invalid_Escape_Sequence(block.data());
                    else if(!is_escapable_symbol(regex[index]))
                        throw Regex_Parser_Exception_Invalid_Escape_Sequence(block.data() + 1);
                    result += this->encode_escape_symbol(regex[index]);
                }
                else if(block[0] == '[')
                {
                    block = next_block(regex.substr(index), '[', ']');
                    if(regex[index + block.size()] == '+')
                    {
                        result += block;
                        result += block;
                        result += '*';
                        index++;
                    }
                    else
                    {
                        result += block;
                    }
                }
                else if(block[0] == '(')
                {
                    std::string_view trimmed_block = block.substr(1, block.size() - 2);
                    std::string expanded_block = expand_plus(trimmed_block);
                    result += '(';
                    result += expanded_block;
                    result += ')';
                    if(regex[index + block.size()] == '+')
                    {
                        result += '(';
                        result += expanded_block;
                        result += ')';
                        result += '*';
                        index++;
                    }
                }
                else if(block[0] == '+')
                {
                    if(index == 0 || (index > 2 && regex[index - 2] != '\\' && regex[index - 1] == ':'))
                    {
                        throw Regex_Parser_Exception_Invalid_Kleene_Plus_Operation(block.data());
                    }
                    result += result.back();
                    result += '*';
                }
                else if(block[0] - '\t' > 2) // != \n or \t
                {
                    if(block[0] == '*')
                        if(index == 0 || (index > 2 && regex[index - 2] != '\\' && regex[index - 1] == ':'))
                        {
                            throw Regex_Parser_Exception_Invalid_Kleene_Star_Operation(block.data());
                        }
                    
                    result += block;
                }
            
                index += block.size();
            } catch(const int)
            {
                throw Regex_Parser_Exception(show_error_in_string_single(std::string("Error no closing bracket for:"), std::string(regex.data(), regex.size()), index));
            }
            
        }
        return result;
    }
    size_t find_state(NFA_State* c)
    {
        size_t index = 0;
        for(; index < states.size() && &states[index] != c; index++){}
        return index == states.size() ? -1 : index;
    }
    void print()
    {
       for(size_t state_index = 0; state_index < this->states.size(); state_index++)
       {
           NFA_State& current_state = states[state_index];
           std::cout<<state_index;
           if(current_state.accept)
                std::cout<<'@'<<current_state.attribute;
           std::cout<<": ";
           for(size_t i = 0; i < current_state.transitions.size(); i++)
           {
               auto& transitions = current_state.transitions;
               if(transitions[i].size() > 0)
               {
                   std::cout<<'{';
                   for(auto it = current_state.transition_iterator(i); it != transitions[i].end(); it++)
                   {
                       std::cout<<((char) i)<<":"<<find_state(*it)<<", ";
                   }
                   std::cout<<'}';
               }
           }
           std::cout<<'\n';
       }
    }
    NFA_State& create_state(std::string_view attribute) noexcept
    {
        states.push_back(NFA_State(state_storage, false, attribute));
        return states.back();
    }
    void trim_block(std::string_view& block, char opening = '(') const noexcept
    {
        if(block[0] == opening)//trim brackets
        {
            block = block.substr(1, block.size() - 2);
        }
    }
    bool is_escaped_symbol(const unsigned char c) const noexcept
    {
        return c - 1 < 8 || c - 14 < 4;
    }
    bool is_escapable_symbol(const char c) const noexcept
    {
        return c == '(' || c == ')' || c == '[' || c == ']' || c == '*' || c == '+'
                    || c == '\\' || c == 'n' || c == 't' || c == '@' || c == '.' || c == ':';
    }
    char encode_escape_symbol(const char c) const noexcept
    {
        char result = 0;
        switch (c)
        {
            case '(':
                result = 1;
            break;
            case ')':
                result = 2;
            break;
            case '[':
                result = 3;
            break;
            case ']':
                result = 4;
            break;
            case '*':
                result = 5;
            break;
            case '\\':
                result = 6;
            break;
            case 'n':
                result = 7;
            break;
            case 't':
                result = 8;
            break;
            case '@':
                result = 14;
            break;
            case '.':
                result = 15;
            break;
            case '+':
                result = 16;
            break;
            case ':':
                result = 17;
            break;
        }
        return result;
    }  
    char decode_escape_symbol(const char c) const noexcept
    {
        char result = 0;
        switch (c)
        {
            case 1:
                result = '(';
            break;
            case 2:
                result = ')';
            break;
            case 3:
                result = '[';
            break;
            case 4:
                result = ']';
            break;
            case 5:
                result = '*';
            break;
            case 6:
                result = '\\';
            break;
            case 7:
                result = '\n';
            break;
            case 8:
                result = '\t';
            break;
            case 14:
                result = '@';
            break;
            case 15:
                result = '.';
            break;
            case 16:
                result = '+';
            break;
            case 17:
                result = ':';
            break;
        }
        return result;
    }
    NFA_State* load_states(std::string_view regex, NFA_State* start, NFA_State* end, std::string_view current_attribute) 
    {
        size_t index = 0;
        while(index < regex.size())
        {
            std::string_view block = next_block(regex.substr(index));
            if(block[0] == '[')
                block = next_block(regex.substr(index), '[', ']');
        
            index += block.size();
            if(index < regex.size() && regex[index] == '|')//add 3 states start with transition into two states
            {
                NFA_State* first_start = &create_state(current_attribute);
                NFA_State* first_end = &create_state(current_attribute);
                load_states(block, first_start, first_end, current_attribute)->add_transition(0, end);
                start->add_transition(0, first_start);
                while(index < regex.size() && regex[index] == '|')
                {
                    index++;
                    std::string_view n_block = regex[index] == '['? next_block(regex.substr(index), '[', ']') : next_block(regex.substr(index));
                    if(n_block.size() > 0)
                    {
                        NFA_State* second_start = &create_state(current_attribute);
                        NFA_State* second_end = &create_state(current_attribute);
                        load_states(n_block, second_start, second_end, current_attribute)->add_transition(0, end);
                        start->add_transition(0, second_start);
                    }
                    index += n_block.size();
                }
            }
            else if(index < regex.size() && regex[index] == '*')
            {
                NFA_State* repeated_start = &create_state(current_attribute);
                NFA_State* repeated_end = &create_state(current_attribute);
                repeated_end = load_states(block, repeated_start, repeated_end, current_attribute);
                repeated_end->set_accepting(false);
                start->add_transition(0, end);
                start->add_transition(0, repeated_start);
                repeated_end->add_transition(0, end);
                end->add_transition(0, start);
                index++;
                start->set_accepting(false);
                end->set_accepting(true);
            }
            else if(block[0] == '[')
            {
                if((block.size() - 2) % 3 != 0)
                    throw Regex_Parser_Exception_Invalid_Range(block.data());
                trim_block(block, '[');
                if(block.size() >= 3 && block.size() % 3 == 0)
                {
                    size_t it = 0;
                    while(it < block.size())
                    {
                        for(char i = block[it]; i <= block[it + 2]; i++)
                        {
                            start->add_transition(i, end);
                        }
                        it += 3;
                    }
                }
                if(index < regex.size() && regex[index] == '*')
                {
                    start->add_transition(0, end);
                    end->add_transition(0, start);
                    index++;
                }
                start->set_accepting(false);
                end->set_accepting(true);
            }
            else if(block[0] == '(')
            {
                trim_block(block);
                end = load_states(block, start, end, current_attribute);
                if(index < regex.size() && regex[index] == '*')
                {
                    start->add_transition(0, end);
                    end->add_transition(0, start);
                    index++;
                }
            }
            else if(block.size() == 1)//concat single symbol
            {
                if(is_escaped_symbol(block[0]))
                {
                    start->add_transition(decode_escape_symbol(block[0]), end);
                }
                else if(block[0] == '.')
                {
                    for(size_t i = 1; i < 128; i++)
                    if(isprint(i))
                        start->add_transition(i, end);
                }
                else
                    start->add_transition(block[0], end);
                
                end->set_attribute(current_attribute);
                start->set_accepting(false);
                end->set_accepting(true);
            }
            if(index < regex.size())
            {
                start = end;
                end = &create_state(current_attribute);
            }
        } 
        
        return end;
    }
};
