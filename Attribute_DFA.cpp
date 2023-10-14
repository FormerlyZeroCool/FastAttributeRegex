#include "Attribute_DFA.hpp"

    Attribute_DFA::Attribute_DFA(std::string&& regex, std::deque<std::string>&& attributes): regex(regex), attributes(attributes)
    {
        for(size_t i = 0; i < attributes.size(); i++)
        {
            attributes_priority[attributes[i]] = i;
        }
    }
    size_t Attribute_DFA::get_priority(std::string_view attribute)
    {
        if(attributes_priority.count(attribute) == 0)
            throw std::string("Error invalid attribute: ") += attribute;
        return attributes_priority[attribute];
    }
    std::deque<Attribute_DFA::DFA_State>& Attribute_DFA::get_states() noexcept
    {
        return states;
    }
    LexToken Attribute_DFA::parse_token(const std::string_view input_text, const size_t start)
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
    void Attribute_DFA::set_regex(const std::string& regex) noexcept
    {
        this->regex = regex;
    } 
    void Attribute_DFA::print()
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
    size_t Attribute_DFA::start_state() const noexcept
    {
        return 0;
    }
    size_t Attribute_DFA::transition(char symbol, size_t current_state) const noexcept
    {
        if(current_state == -1)
            return -1;
        return states[current_state].transitions.get(symbol);
    }
    Attribute_DFA Attribute_DFA::minimize()
    {
        if(group_states_by_distiguishibility().size() < states.size())
            return remove_indistinguishible_states().minimize();
        return remove_indistinguishible_states();
    }
    Attribute_DFA Attribute_DFA::remove_indistinguishible_states()
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
    std::vector<std::pair<size_t, std::list<size_t> > > Attribute_DFA::group_states_by_distiguishibility()
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
    Fast_Attribute_DFA Attribute_DFA::to_fast_dfa() const noexcept
    {
        Fast_Attribute_DFA dfa(regex, attributes);
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
