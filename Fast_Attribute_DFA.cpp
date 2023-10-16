#include "Attribute_DFA.hpp"

//states code
//runtime length encode state transitions
        std::ostream& Fast_Attribute_DFA::DFA_State::write(std::ostream& o)
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
        std::istream& Fast_Attribute_DFA::DFA_State::read(std::istream& in)
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

//dfa code

std::ostream& Fast_Attribute_DFA::write(std::ostream& o)
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
    std::istream& Fast_Attribute_DFA::read_string(std::istream& in, std::string& str)
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
    std::istream& Fast_Attribute_DFA::read(std::istream& in)
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
    void Fast_Attribute_DFA::load_file(const std::string filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        read(file);
    }
    void Fast_Attribute_DFA::write_to_file(const std::string filepath)
    {
        std::ofstream file(filepath, std::ios::binary);
        write(file);
    }

    Fast_Attribute_DFA::Fast_Attribute_DFA(const std::string& regex, const std::deque<std::string>& attributes): regex(regex)
    {
        for(size_t i = 0; i < attributes.size(); i++)
            this->attributes.push_back(attributes[i]);
    }
    bool Fast_Attribute_DFA::operator==(const Fast_Attribute_DFA& o) const noexcept
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
    std::ostream& operator<<(std::ostream& o, const Fast_Attribute_DFA& dfa)
    {
        for(size_t state_index = 0; state_index < dfa.states.size(); state_index++)
        {
            const Fast_Attribute_DFA::DFA_State& state = dfa.states[state_index];
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
    size_t Fast_Attribute_DFA::start_state() const noexcept
    {
        return 0;
    }
    uint32_t Fast_Attribute_DFA::transition(char symbol, uint32_t current_state) const noexcept
    {
        if(current_state == -1)
            return -1;
        return states[current_state].transitions[symbol];
    }
    const std::vector<Fast_Attribute_DFA::DFA_State>& Fast_Attribute_DFA::get_states() const noexcept
    {
        return states;
    }
    const std::string& Fast_Attribute_DFA::get_attribute(const DFA_State& state) const noexcept
    {
        return attributes[state.attribute_priority];
    }

    Fast_Attribute_DFA::ParseRecord Fast_Attribute_DFA::parse_token_simple(const std::string_view input_text, const size_t start)
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
    SimpleLexToken Fast_Attribute_DFA::parse_token_no_attribute(const std::string_view input_text, const size_t start)
    {
        const auto rec = parse_token_simple(input_text, start);
        if(states[rec.state].accept & (rec.state != -1))
        {
            SimpleLexToken tok(start, this->get_states()[rec.state].attribute_priority, std::string_view(input_text.data() + start, rec.end_index - start - 1));
            return tok;
        }
        return SimpleLexToken(start, 0, "");
        
    }
    LexToken Fast_Attribute_DFA::parse_token(const std::string_view input_text, const size_t start)
    {
        const auto rec = parse_token_simple(input_text, start);
        if(states[rec.state].accept & (rec.state != -1))
        {
            LexToken tok(start, this->get_states()[rec.state].attribute_priority, attributes[this->get_states()[rec.state].attribute_priority], std::string_view(input_text.data() + start, rec.end_index - start - 1));
            return tok;
        }
        return LexToken(start, 0, "", "");
        
    }