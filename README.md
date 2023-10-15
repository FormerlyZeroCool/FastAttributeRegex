# FastAttributeRegex

A small library that implements attributable Unix style regex with deterministic finite automata<br>
<br>
Designed for create dynamic Lexicographic analyzers that are fast, and easily integrated with C++ programs<br>
<br>
To use the library, compile the statically linkable library file with the `make` command, and include the Regex.hpp header type:<br>

```bash
make run #to build, and run sample project
#or 
make # to build unix style statically linkable library, and sample project
```

Then you can use the `compile_regex` function to generate an optimized finite automata for the regex given to it as a string parameter<br>
Here's an example of how to create a finite automata from a simple input regex string, see sample_main for how to read it from a file easily, and supply larger regex with multiple attributes as input:

```cpp
    Fast_Attribute_DFA dfa = compile_regex(std::string("1*0"));
```
Then to parse regex tokens from an input string use the parseToken method on the Fast_Attribute_DFA class as follows:
```cpp
    size_t index = 0;
    std::string input_text = "1110";
    LexToken tok = dfa.parse_token(input_text, index);
```
The above code using the dfa we created in the previous snippet will return a token with the entire string as the text of the LexToken returned<br>
<br>
Example attribute regex:

```c
@keyword:(asm)|(if)|(else)|(for)|(while)|(extern)
@id:(_|[a-zA-Z])(_|[0-9a-zA-Z])*
@number:([0-9]+)|([0-9]*\.[0-9]*)
```
<br>
The above code defines three classes of attributes named "keyword" "id" and "number" respectively. <br>
In the LexToken, which is returned by the method parseToken they will have a string_view containing that attribute.<br>
As well as a priority corresponding to the attribute, where "keyword" will have priority 1, "id" priority 2, and "number" priority 3 and so on for other classes.<br>
The LexToken will also contain the index in the source text the token was parsed from as well as the actual text of the parsed token.<br>
<br>
Below is the structure of the LexToken described above:

```cpp
struct LexToken {
    //the index is the index into the source text from which this token was parsed
    size_t index = 0;
    size_t priority = 0;
    std::string_view attribute;
    std::string_view text;
    LexToken(size_t index, size_t priority, std::string_view attribute, std::string_view text): 
        priority(priority), attribute(attribute), text(text) {}
    friend std::ostream& operator<<(std::ostream& o, const LexToken& tok)
    {
        o<<"<"<<tok.attribute<<":"<<tok.priority<<"->"<<tok.text<<">";
        return o;
    }
};
```