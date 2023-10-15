#include "Attribute_NFA.hpp"

std::pair<size_t, std::string> trim_to_line_on_index(const std::string& text, const size_t index, char delimiter)
{
    size_t last_delimiter = 0;
    size_t current_delimiter = 0;
    while(current_delimiter < text.size() && current_delimiter < index)
    {
        last_delimiter = current_delimiter;
        current_delimiter = text.find(delimiter, last_delimiter + 1);
    }
    return std::make_pair(last_delimiter + (last_delimiter != 0), text.substr(last_delimiter, current_delimiter < text.size() ? current_delimiter - last_delimiter : text.size() - last_delimiter));
}
std::string show_error_in_string_single(std::string error_message, std::string erroneous_text, size_t index_of_error, char delimiter)
{
    std::string result;
    result += error_message;
    result += '\n';
    const auto trim_info = trim_to_line_on_index(erroneous_text, index_of_error, delimiter);
    result += trim_info.second;
    index_of_error -= trim_info.first;//adjust index to compensate for trimmed text
    result += '\n';
    for(size_t i = 0; i < index_of_error; i++)
    {
        result += ' ';
    }
    result += '^';
    return result;
}
std::string_view get_attribute(const std::string_view regex, size_t index)
{
    const size_t start = ++index;
    size_t walker = start + 1;
    while(walker < regex.size() && regex[walker] != ':')
    { walker++; }
    return regex.substr(start, walker - start);
}
std::string_view next_block(std::string_view regex, char opening, char closing)
{
    if(regex[0] == opening)
    {
        size_t length = 1, depth = 1;

        while(depth && length < regex.size())
        {
            depth += (regex[length] == opening) - (regex[length] == closing);
            length++;
        }
        if(depth != 0)
            throw 1;
        return regex.substr(0, length);
    }
    else
        return regex.substr(0, 1);
}


Attribute_NFA::Attribute_NFA(std::string regex): regex(regex)
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
bool Attribute_NFA::accepting_compound_state(const std::set<NFA_State*>& compound_state)
    {
        for(auto it = compound_state.begin(); it != compound_state.end(); it++)
        {
            if((*it)->accept)
                return true;
        }
        return false;
    }
size_t Attribute_NFA::highest_priority_attribute(const std::set<NFA_State*>& compound_state, std::unordered_map<std::string_view, size_t>& attributes_priority)
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
Fast_Attribute_DFA Attribute_NFA::to_DFA()//invalidates this NFA regex, and atrributes
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
std::string Attribute_NFA::expand_plus(std::string_view regex)
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
size_t Attribute_NFA::find_state(NFA_State* c)
    {
        size_t index = 0;
        for(; index < states.size() && &states[index] != c; index++){}
        return index == states.size() ? -1 : index;
    }
void Attribute_NFA::print()
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
NFA_State& Attribute_NFA::create_state(std::string_view attribute) noexcept
    {
        states.push_back(NFA_State(state_storage, false, attribute));
        return states.back();
    }
void Attribute_NFA::trim_block(std::string_view& block, char opening) const noexcept
    {
        if(block[0] == opening)//trim brackets
        {
            block = block.substr(1, block.size() - 2);
        }
    }
bool Attribute_NFA::is_escaped_symbol(const unsigned char c) const noexcept
    {
        return c - 1 < 8 || c - 14 < 4;
    }
bool Attribute_NFA::is_escapable_symbol(const char c) const noexcept
    {
        return c == '(' || c == ')' || c == '[' || c == ']' || c == '*' || c == '+'
                    || c == '\\' || c == 'n' || c == 't' || c == '@' || c == '.' || c == ':';
    }
char Attribute_NFA::encode_escape_symbol(const char c) const noexcept
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
char Attribute_NFA::decode_escape_symbol(const char c) const noexcept
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
NFA_State* Attribute_NFA::load_states(std::string_view regex, NFA_State* start, NFA_State* end, std::string_view current_attribute) 
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
                //end->set_accepting(true);
                start = end;
                end = &create_state(current_attribute);
                start->set_accepting(false);
                end->set_accepting(true);
                start->add_transition(0, end);
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
