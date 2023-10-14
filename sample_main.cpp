#include "Regex.hpp"

void test_2(Fast_Attribute_DFA& dfa, std::string_view input_text)
{
    size_t index = 0;
    size_t token_count = 0;
    while(index < input_text.size())
    {
        LexToken t = dfa.parse_token(input_text, index);
        index += t.text.size();
        
        index += (t.text.size() == 0);
        if(t.priority != 16 && t.text.size() > 0)
        {
            std::cout<<token_count<<" "<<t<<'\n';
            token_count++;
        }
    }
    std::cout<<token_count<<"\n";
}

int main(int argc, char ** argv)
{
    Fast_Attribute_DFA dfa(compile_regex(read_file("example.txt")));
    Fast_Attribute_DFA dfa2(compile_regex(std::string("")));
    
    dfa.write_to_file("test_binary_fadfa.bin");
    
    dfa2.load_file("test_binary_fadfa.bin");
    std::cout<<dfa<<'\n';
    const std::string test_input = read_file("testinput.c");
    test_2(dfa2, test_input);
    return 0;
}