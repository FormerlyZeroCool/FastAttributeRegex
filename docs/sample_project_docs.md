# Example Project Documentation

## Overview

This example project showcases how to perform lexical analysis using regular expressions. It employs the `Regex.hpp` module to compile regular expressions and generate deterministic finite automata (DFA) with attributes. The program then utilizes these DFA to tokenize input text.

## Functions

### `example_lexing_entire_string`

```cpp
void example_lexing_entire_string(Fast_Attribute_DFA& dfa, std::string_view input_text)
```

The `example_lexing_entire_string` function demonstrates how to tokenize an entire input string using a DFA generated from an attribute regex. It takes a DFA and an input text as arguments and prints the tokens along with their indices.

- **Parameters**:
  - `dfa`: A reference to a `Fast_Attribute_DFA` object generated from the attribute regex.
  - `input_text`: A `std::string_view` representing the input text to be tokenized.

### Usage

1. Call `example_lexing_entire_string(dfa, input_text)` with the appropriate arguments.

### Behavior

1. The function initializes an index variable (`index`) to keep track of the current position in the input text.

2. It also initializes a variable (`token_count`) to count the number of tokens found.

3. The function enters a loop, continuing as long as there is unprocessed input text.

4. In each iteration, it uses the DFA (`dfa`) to parse a token from the input text starting at the current index (`index`). The result is stored in a variable `t`.

5. The index is then updated to point to the end of the token in the input text.

6. If the token's text is empty, the index is incremented again to ensure progress.

7. If the token is not a priority 16 token (indicating it's not a whitespace character that the lexemes.txt file skips with this classification) and has non-empty text, it is printed to the console along with its index and text.

8. The token count is incremented.

9. Once the loop finishes, the total number of tokens found is printed to the console.

## Main Function (`main`)

The `main` function serves as the entry point of the program. It takes command-line arguments to specify the path to the attribute regex file and the file to be tokenized.

- **Parameters**:
  - `argc`: The number of command-line arguments.
  - `argv`: An array of character pointers containing the arguments.

### Usage

1. Ensure that the program is compiled with the necessary files, including `Regex.hpp`.

2. Execute the compiled binary, providing the path to the attribute regex file and the file to be tokenized as command-line arguments.

**Example Usage**:

```bash
./lexing_example attribute_regex.txt input_file.txt
```

### Behavior

1. If the number of command-line arguments is less than 3, an error message is printed, and the program exits with a status code of 1.

2. The attribute regex file specified by `argv[1]` is read and used to generate an NFA (Non-deterministic Finite Automaton). Information about this NFA is then printed to the console.

3. The attribute regex is compiled into a DFA (Deterministic Finite Automaton) and stored in the variable `dfa`.

4. An empty DFA, `dfa2`, is created to demonstrate how to load a DFA from a file. It is then saved to a binary file named `test_binary_fadfa.bin`.

5. The saved DFA is loaded from the file `test_binary_fadfa.bin` and stored in `dfa2`.

6. Information about the loaded DFA is printed to the console.

7. The input file specified by `argv[2]` is read and tokenized using `dfa2`.

8. The resulting tokens, along with their indices, are printed to the console.

## Important Notes

- Ensure that the `Regex.hpp` file and any required dependencies are available and correctly included in your project.

- The program first generates an NFA from the provided attribute regex file, prints information about it, and then compiles it into a DFA.

- It demonstrates how to save the DFA to a binary file (`test_binary_fadfa.bin`) and subsequently load it from the file.

- Finally, it tokenizes the input file using the loaded DFA and prints the resulting tokens to the console.