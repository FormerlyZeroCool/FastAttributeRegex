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

class Attribute_NFA;
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
        std::ostream& write(std::ostream& o)
        {
            uint32_t count_same = 0;
            uint32_t current_state = this->transitions[0];
            o.write(reinterpret_cast<char*>(&this->attribute_priority), sizeof(this->attribute_priority));
            o.write(reinterpret_cast<char*>(&this->accept), sizeof(this->accept));
            for(size_t i = 0; i < this->transitions.size(); i++)
            {
                if(this->transitions[i] != current_state)
                {
                    if(count_same > 1)
                    {
                        o.write(reinterpret_cast<const char*>(&DFA_State::rle_key), sizeof(DFA_State::rle_key));
                        o.write(reinterpret_cast<char*>(&count_same), sizeof(count_same));
                        o.write(reinterpret_cast<char*>(&current_state), sizeof(current_state));
                    }
                    else   
                        o.write(reinterpret_cast<char*>(&current_state), sizeof(current_state));

                    current_state = this->transitions[i];
                    count_same = 1;
                }
                else
                    count_same++;
            }
            if(count_same > 1)
            {
                o.write(reinterpret_cast<const char*>(&DFA_State::rle_key), sizeof(DFA_State::rle_key));
                o.write(reinterpret_cast<char*>(&count_same), sizeof(count_same));
                o.write(reinterpret_cast<char*>(&current_state), sizeof(current_state));
            }
            else   
                o.write(reinterpret_cast<char*>(&current_state), sizeof(current_state));

            o.write(reinterpret_cast<const char*>(&DFA_State::end_state_encoding), sizeof(DFA_State::end_state_encoding));
            return o;
        }
        std::istream& read(std::istream& in)
        {
            in.read(reinterpret_cast<char*>(&this->attribute_priority), sizeof(this->attribute_priority));
            in.read(reinterpret_cast<char*>(&this->accept), sizeof(this->accept));
            uint32_t current = 0;
            uint32_t state_index = 0;
            while(in)
            {
                in.read(reinterpret_cast<char*>(&current), sizeof(current));
                if(current == DFA_State::end_state_encoding)
                    break;
                if(state_index >= this->transitions.size())
                    break;//should throw exception
                if(current == DFA_State::rle_key)
                {
                    uint32_t count_same, transition;
                    in.read(reinterpret_cast<char*>(&count_same), sizeof(count_same));
                    in.read(reinterpret_cast<char*>(&transition), sizeof(transition));
                    for(size_t i = 0; state_index < this->transitions.size() && i < count_same; i++, state_index++)
                    {
                        this->transitions[state_index] = transition;
                    }
                }
                else
                {
                    this->transitions[state_index++] = current;
                }
            }
            if(current != DFA_State::end_state_encoding)
                std::cerr<<"Corrupted file no end state encoding after state\n";
            
            return in;
        }
    };
    std::ostream& write(std::ostream& o)
    {
        uint32_t size = this->states.size();
        o.write(reinterpret_cast<char*>(&size), sizeof(size));
        size = this->attributes.size();
        o.write(reinterpret_cast<char*>(&size), sizeof(size));
        size = this->regex.size();
        o.write(reinterpret_cast<char*>(&size), sizeof(size));
        o.write(this->regex.c_str(), size);
        for(size_t i = 0; i < this->attributes.size(); i++)
        {
            size = this->attributes[i].size();
            o.write(reinterpret_cast<char*>(&size), sizeof(size));
            o.write(this->attributes[i].c_str(), this->attributes[i].size());
        }
        for(size_t i = 0; i < this->states.size(); i++)
            this->states[i].write(o);
        return o;
    }
    std::istream& read_string(std::istream& in, std::string& str)
    {
        uint32_t size = 0;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        str.clear();
        str.reserve(size);
        for(size_t i = 0; i < size; i++)
            str += ' ';

        in.read(str.data(), size);
        return in;
    }
    std::istream& read(std::istream& in)
    {
        uint32_t states_count = 0, attributes_count = 0;
        in.read(reinterpret_cast<char*>(&states_count), sizeof(states_count));
        in.read(reinterpret_cast<char*>(&attributes_count), sizeof(attributes_count));
        read_string(in, regex);
        std::string line;
        this->attributes.clear();
        for(size_t i = 0; i < attributes_count; i++)
        {
            read_string(in, line);
            this->attributes.push_back(line);
        }
        this->states.clear();
        this->states.reserve(states_count);
        for(size_t i = 0; i < states_count; i++)
        {
            this->states.push_back(DFA_State(false, "", 0));
            this->states.back().read(in);
        }
        return in;
    }
    void load_file(const std::string filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        read(file);
    }
    void write_to_file(const std::string filepath)
    {
        std::ofstream file(filepath, std::ios::binary);
        write(file);
    }
    std::vector<DFA_State> states;
    std::string regex;
    std::vector<std::string> attributes;
    Fast_Attribute_DFA(std::string&& regex, std::deque<std::string>&& attributes): regex(regex)
    {
        for(size_t i = 0; i < attributes.size(); i++)
            this->attributes.push_back(attributes[i]);
    }
    bool operator==(const Fast_Attribute_DFA& o) const
    {
        bool equal = states.size() == o.states.size() && regex.size() == o.regex.size() && attributes.size() == o.attributes.size();
        for(size_t i = 0; equal && i < states.size(); i++)
        {
            const DFA_State& state = states[i];
            const DFA_State& ostate = o.states[i];
            equal = state == ostate;
        }
        return equal;
    }
    friend std::ostream& operator<<(std::ostream& o, const Fast_Attribute_DFA& dfa)
    {
        for(size_t state_index = 0; state_index < dfa.states.size(); state_index++)
        {
            const DFA_State& state = dfa.states[state_index];
            std::cout<<state_index<<(state.is_accepting() ? "@":"")<<(state.is_accepting() ? dfa.get_attribute(state):"")<<": {";
            for(size_t i = ' '; i < 128; i++)
            {
                if(state.transition_iterator(i) != -1)
                    std::cout<<((char) i)<<":"<<state.transition_iterator(i)<<", ";
            }
            std::cout<<"}\n";
        }
        return o;
    }
    size_t start_state() const noexcept
    {
        return 0;
    }
    uint32_t transition(char symbol, uint32_t current_state) const noexcept
    {
        if(current_state == -1)
            return -1;
        return states[current_state].transitions[symbol];
    }
    const std::vector<DFA_State>& get_states() const noexcept
    {
        return states;
    }
    const std::string& get_attribute(const DFA_State& state) const noexcept
    {
        return attributes[state.attribute_priority];
    }
    struct ParseRecord {
        const size_t state, end_index;
        ParseRecord(const size_t state, const size_t end): state(state), end_index(end) {}
    };
    ParseRecord parse_token_simple(const std::string_view input_text, const size_t start)
    {

        uint32_t state = this->start_state();
        uint32_t last_state = -1;
        size_t input_index = start;
        const char original_ending = (*(const_cast<char*>(input_text.data()) + input_text.size()));
        (*(const_cast<char*>(input_text.data()) + input_text.size())) = 0;
        while(state != -1 && input_index <= input_text.size())
        {
            last_state = state;
            state = this->states[state].transitions[input_text[input_index]];
            input_index++;
            if(state != -1 && input_index == input_text.size())
                last_state = state;
            if(state != -1 && this->get_states()[last_state].accept && this->get_states()[state].attribute_priority != this->get_states()[last_state].attribute_priority && this->get_states()[state].attribute_priority > this->get_states()[last_state].attribute_priority)
            {
                last_state = state;
                state = -1;
            }
        }
        (*(const_cast<char*>(input_text.data()) + input_text.size())) = original_ending;

        return ParseRecord(last_state, input_index);
    }
    SimpleLexToken parse_token_no_attribute(const std::string_view input_text, const size_t start)
    {
        const auto rec = parse_token_simple(input_text, start);
        if(states[rec.state].accept & (rec.state != -1))
        {
            SimpleLexToken tok(start, this->get_states()[rec.state].attribute_priority, std::string_view(input_text.data() + start, rec.end_index - start - 1));
            return tok;
        }
        return SimpleLexToken(start, 0, "");
        
    }
    LexToken parse_token(const std::string_view input_text, const size_t start)
    {
        const auto rec = parse_token_simple(input_text, start);
        if(states[rec.state].accept & (rec.state != -1))
        {
            LexToken tok(start, this->get_states()[rec.state].attribute_priority, attributes[this->get_states()[rec.state].attribute_priority], std::string_view(input_text.data() + start, rec.end_index - start - 1));
            return tok;
        }
        return LexToken(start, 0, "", "");
        
    }
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
    Attribute_DFA(std::string&& regex, std::deque<std::string>&& attributes): regex(regex), attributes(attributes)
    {
        for(size_t i = 0; i < attributes.size(); i++)
        {
            attributes_priority[attributes[i]] = i;
        }
    }
    size_t get_priority(std::string_view attribute)
    {
        if(attributes_priority.count(attribute) == 0)
            throw std::string("Error invalid attribute: ") += attribute;
        return attributes_priority[attribute];
    }
    std::deque<DFA_State>& get_states() noexcept
    {
        return states;
    }
    LexToken parse_token(const std::string_view input_text, const size_t start)
    {
        size_t state = this->start_state();
        size_t last_state = -1;
        size_t input_index = start;
        const char original_ending = (*(const_cast<char*>(input_text.data()) + input_text.size()));
        (*(const_cast<char*>(input_text.data()) + input_text.size())) = 0;
        while(state != -1 && input_index <= input_text.size())
        {
            last_state = state;
            state = this->states[state].transitions[input_text[input_index]];
            input_index++;
            if(state != -1 && input_index == input_text.size())
                last_state = state;
            if(state != -1 && this->get_states()[last_state].accept && this->get_states()[state].attribute_priority != this->get_states()[last_state].attribute_priority && this->get_states()[state].attribute_priority > this->get_states()[last_state].attribute_priority)
            {
                last_state = state;
                state = -1;
            }
        }
        (*(const_cast<char*>(input_text.data()) + input_text.size())) = original_ending;
        if(last_state != -1)
        {
            LexToken tok(start, this->get_states()[last_state].attribute_priority, this->get_states()[last_state].attribute, std::string_view(input_text.data() + start, input_index - start - 1));
            return tok;
        }
        return LexToken(start, 0, "", "");
        
    }

    void set_regex(const std::string& regex) noexcept
    {
        this->regex = regex;
    } 
    void print()
    {
        for(size_t state_index = 0; state_index < states.size(); state_index++)
        {
            DFA_State& state = states[state_index];
            std::cout<<state_index<<(state.is_accepting() ? "@":"")<<(state.is_accepting() ? state.attribute:"")<<": {";
            for(size_t i = ' '; i < 128; i++)
            {
                if(state.transition_iterator(i) != -1)
                    std::cout<<((char) i)<<":"<<state.transition_iterator(i)<<", ";
            }
            std::cout<<"}\n";
        }
    }
    size_t start_state() const noexcept
    {
        return 0;
    }
    size_t transition(char symbol, size_t current_state) const noexcept
    {
        if(current_state == -1)
            return -1;
        return states[current_state].transitions.get(symbol);
    }
    Attribute_DFA minimize()
    {
        if(group_states_by_distiguishibility().size() < states.size())
            return remove_indistinguishible_states().minimize();
        return remove_indistinguishible_states();
    }
    Attribute_DFA remove_indistinguishible_states()
    {
        std::vector<size_t> offset_at_index(this->states.size());
        std::vector<std::pair<size_t, std::list<size_t> > > grouped_states = group_states_by_distiguishibility();
        for(size_t group_id = 0; group_id < grouped_states.size(); group_id++)
        {
            std::pair<size_t, std::list<size_t> >& group = grouped_states[group_id];
            for(auto group_it = group.second.begin(); group_it != group.second.end(); group_it++)
            {
                offset_at_index[*group_it] = group_id;
            }
        }
        Attribute_DFA minimized(std::move(this->regex), std::move(this->attributes));
        for(size_t group_id = 0; group_id < grouped_states.size(); group_id++)
        {
            const DFA_State& example_state = states[*grouped_states[group_id].second.begin()];
            minimized.states.push_back(DFA_State(minimized.state_storage, example_state.accept, example_state.attribute, example_state.attribute_priority));
            for(size_t i = 0; i < example_state.transitions.size(); i++)
            {
                if(example_state.transitions.get(i) != -1)
                    minimized.states.back().set_transition(i, offset_at_index[example_state.transitions.get(i)]);
            }
        }
        return minimized;
    }
    //return vector of hashes, and values 
    std::vector<std::pair<size_t, std::list<size_t> > > group_states_by_distiguishibility()
    {
        std::vector<std::pair<size_t, std::list<size_t> > > groups(states.size(), std::make_pair(0, std::list<size_t>()));
        for(size_t i = 0; i < states.size(); i++)
        {
            const DFA_State& state = states[i];
            const size_t hash = state.hash();
            //first check if state goes into existing group
            bool state_found = false;
            for(size_t group_index = 0; !state_found && group_index < groups.size(); group_index++)
            {
                std::pair<size_t, std::list<size_t> > & group = groups[group_index];
                if(group.first == hash && states[*group.second.begin()] == state)
                {
                    group.second.insert(group.second.end(), i);
                    break;
                }
                else if(group.second.size() == 0)
                {
                    group.second.insert(group.second.end(), i);
                    group.first = hash;
                    break;
                }
            }
        }
        size_t valid_elements = 0;
        for(; valid_elements < groups.size(); valid_elements++)
        {
            if(groups[valid_elements].second.size() == 0)
            {
                valid_elements--;
                groups.pop_back();
            }
        }
        return groups;
    }
    Fast_Attribute_DFA to_fast_dfa()
    {
        Fast_Attribute_DFA dfa(std::move(regex), std::move(attributes));
        for(size_t state_index = 0; state_index < states.size(); state_index++)
        {
            const DFA_State& state = states[state_index];
            dfa.states.push_back(Fast_Attribute_DFA::DFA_State(state.is_accepting(), state.attribute, state.attribute_priority));
            for(size_t i = 0; i < state.transitions.size(); i++)
            {
                dfa.states.back().set_transition(i, state.transitions.get(i));
            }
        }
        return dfa;
    }
};
