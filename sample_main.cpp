#include "Regex.hpp"

void example_lexing_entire_string(Fast_Attribute_DFA& dfa, std::string_view input_text)
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
    if(argc < 3)
    {
        std::cerr<<"Error must supply path to attribute regex to be parsed in first param, and file to be lexed in second\n";
        exit(1);
    }
    Attribute_NFA nfa(read_file(argv[1]));
    std::cout<<"\n\nNFA generated from file: "<<argv[1]<<":\n\n";
    nfa.print();
    Fast_Attribute_DFA dfa = compile_regex(read_file(argv[1]));
    //created empty dfa to show how to load dfa from file
    Fast_Attribute_DFA dfa2 = compile_regex(std::string(""));
    
    dfa.write_to_file("test_binary_fadfa.bin");
    
    dfa2.load_file("test_binary_fadfa.bin");


    std::cout<<"\n\n\nDFA generated from file: "<<argv[1]<<":\n\n";
    std::cout<<dfa2<<'\n';

    std::string file_to_be_lexed = argv[2];
    const std::string test_input = read_file(file_to_be_lexed);
    std::cout<<"\nTokens parsed by performing lexical analysis on file: "<<file_to_be_lexed<<" with dfa generated:\n";
    example_lexing_entire_string(dfa2, test_input);
    return 0;
}