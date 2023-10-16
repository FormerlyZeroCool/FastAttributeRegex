# Attribute_NFA Documentation

## Overview

The `Attribute_NFA` class is a C++ implementation of a Non-Deterministic Finite Automaton (NFA) for processing regular expressions with attribute annotations. It provides methods for creating, manipulating, and converting NFAs. This documentation will cover the major components and functionalities of the class.

## Functions

### `trim_to_line_on_index`

```cpp
std::pair<size_t, std::string> trim_to_line_on_index(const std::string& text, const size_t index, char delimiter)
```

This function takes a `text` string, an `index`, and a `delimiter` character. It returns a pair containing the position of the last `delimiter` before `index` and the substring from the last delimiter to the next delimiter or the end of the string.

### `show_error_in_string_single`

```cpp
std::string show_error_in_string_single(std::string error_message, std::string erroneous_text, size_t index_of_error, char delimiter)
```

This function generates an error message to highlight a specific error in a given `erroneous_text`. It marks the position of the error using a caret (^) character.

### `get_attribute`

```cpp
std::string_view get_attribute(const std::string_view regex, size_t index)
```

This function extracts the attribute name from a regular expression starting at a specific `index`.

### `next_block`

```cpp
std::string_view next_block(std::string_view regex, char opening, char closing)
```

This function identifies the next block delimited by `opening` and `closing` characters in the `regex` string.

### `NFA_State` Constructor

```cpp
NFA_State::NFA_State(std::vector<std::set<NFA_State*>>& storage, bool accept, std::string_view attribute)
```

This constructor initializes an NFA state with the provided `accept` flag and `attribute` view. It also initializes the state's transition storage.

### `NFA_State::add_transition`

```cpp
void NFA_State::add_transition(char input, NFA_State* new_state) noexcept
```

This method adds a transition from the current state to `new_state` on the given `input`.

### `NFA_State::transition_iterator`

```cpp
auto NFA_State::transition_iterator(char input) noexcept
```

This method returns an iterator to the set of states reachable from the current state on the given `input`.

### `NFA_State::is_accepting`

```cpp
bool NFA_State::is_accepting() const noexcept
```

This method returns true if the current state is an accepting state.

### `NFA_State::set_accepting`

```cpp
void NFA_State::set_accepting(bool is_accepting) noexcept
```

This method sets the accepting flag for the current state.

### `NFA_State::set_attribute`

```cpp
void NFA_State::set_attribute(std::string_view attribute) noexcept
```

This method sets the attribute for the current state.

### `Attribute_NFA` Constructor

```cpp
Attribute_NFA::Attribute_NFA(std::string regex)
```

This constructor initializes an Attribute_NFA with the provided regular expression `regex`. It processes the regex to construct the NFA.

### `Attribute_NFA::to_DFA`

```cpp
Fast_Attribute_DFA Attribute_NFA::to_DFA()
```

This method converts the NFA to a Fast_Attribute_DFA.

### `Attribute_NFA::expand_plus`

```cpp
std::string Attribute_NFA::expand_plus(std::string_view regex)
```

This method expands the plus (+) operation in the regular expression.

### `Attribute_NFA::find_state`

```cpp
size_t Attribute_NFA::find_state(NFA_State* c)
```

This method finds the index of a given `NFA_State` within the `states` vector.

### `Attribute_NFA::print`

```cpp
void Attribute_NFA::print()
```

This method prints the details of the NFA, including states, transitions, and attributes.

### `Attribute_NFA::create_state`

```cpp
NFA_State& Attribute_NFA::create_state(std::string_view attribute) noexcept
```

This method creates a new NFA state with the specified `attribute`.

### `Attribute_NFA::trim_block`

```cpp
void Attribute_NFA::trim_block(std::string_view& block, char opening) const noexcept
```

This method trims brackets from the beginning and end of a `block`.

### `Attribute_NFA::is_escaped_symbol`

```cpp
bool Attribute_NFA::is_escaped_symbol(const unsigned char c) const noexcept
```

This method checks if a given character is an escaped symbol.

### `Attribute_NFA::is_escapable_symbol`

```cpp
bool Attribute_NFA::is_escapable_symbol(const char c) const noexcept
```

This method checks if a character can be escaped.

### `Attribute_NFA::encode_escape_symbol`

```cpp
char Attribute_NFA::encode_escape_symbol(const char c) const noexcept
```

This method encodes an escaped symbol.

### `Attribute_NFA::decode_escape_symbol`

```cpp
char Attribute_NFA::decode_escape_symbol(const char c) const noexcept
```

This method decodes an escaped symbol.

## Usage

The `Attribute_NFA` class allows for the creation and manipulation of NFAs to process regular expressions with attribute annotations. The provided methods facilitate various operations such as conversion to DFAs and printing the details of the NFA.

**Note:** This documentation provides an overview of the class and its functionalities. It's essential to review and understand the code implementation for complete comprehension and effective usage.