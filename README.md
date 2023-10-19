# FastAttributeRegex

**A small library that implements attributable Unix style regex with deterministic finite automata**

**Designed for creating dynamic Lexicographic analyzers that are fast, and easily integrated with C++ programs**

## Documentation for the supported regex operators `?`, `*`, `+`, `[]`, and `|`:

1. **`?` - Zero or One Occurrence:**
   - The `?` operator matches the preceding character or group zero or one time. It indicates that the preceding element is optional.
   - For example, `colou?r` matches both "color" and "colour" because the `u` is optional.

2. **`*` - Zero or More Occurrences:**
   - The `*` operator matches the preceding character or group zero or more times. It allows for zero or more repetitions of the preceding element.
   - For example, `go*gle` matches "ggle", "gogle", "google", and so on.

3. **`+` - One or More Occurrences:**
   - The `+` operator matches the preceding character or group one or more times. It requires at least one occurrence of the preceding element.
   - For example, `go+gle` matches "gogle", "google", and so on, but not "ggle".

4. **`[]` - Character Class:**
   - The `[]` notation defines a character class, which allows you to specify a set of characters to match.
   - For example, `[aeiou]` matches any vowel, and `[a-z]` matches any lowercase letter.

5. **`|` - Alternation:**
   - The `|` operator represents alternation and allows you to specify alternatives for matching.
   - For example, `(grape)|(apple)` matches either "grape" or "apple".

These operators provide powerful ways to define patterns for matching strings in regular expressions. They allow for flexibility in specifying the structure of the text you want to search for. Remember that regex patterns can be combined to create complex search patterns.<br>
<br>

## To use the library, compile the statically linkable library file with the `make` command, and include the `Regex.hpp` header type:

```bash
make run #to build, and run sample project
#or 
make # to build unix style statically linkable library, and sample project
```

Then you can use the `compile_regex` function to generate an optimized finite automata for the regex given to it as a string parameter<br>
Here's an example of how to create a finite automata from a simple input regex string, see `sample_main.cpp` for how to read it from a file easily, and supply larger regex with multiple attributes as input:

```cpp
    Fast_Attribute_DFA dfa = compile_regex(std::string("1*0"));
```
Then to parse regex tokens from an input string use the `parseToken` method on the `Fast_Attribute_DFA` class as follows:
```cpp
    size_t index = 0;
    std::string input_text = "1110";
    LexToken tok = dfa.parse_token(input_text, index);
```
The above code using the `dfa` we created in the previous snippet will return a token with the entire string as the text of the `LexToken` returned<br>
<br>
Example attribute regex:

```c
@keyword:(asm)|(if)|(else)|(for)|(while)|(extern)
@id:(_|[a-zA-Z])(_|[0-9a-zA-Z])*
@number:([0-9]+)|([0-9]*\.[0-9]*)
```
<br>
The above code defines three classes of attributes named `"keyword" "id" and "number"` respectively. <br>
In the LexToken, which is returned by the method parseToken they will have a string_view containing that attribute.<br>
As well as a priority corresponding to the attribute, where "keyword" will have priority 1, "id" priority 2, and "number" priority 3 and so on for other classes.<br>
The `LexToken` will also contain the index in the source text the token was parsed from as well as the actual text of the parsed token.<br>
<br>
Below is the structure of the `LexToken` described above:

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