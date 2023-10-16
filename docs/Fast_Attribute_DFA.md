# Fast_Attribute_DFA Documentation

## Overview

The `Fast_Attribute_DFA` class represents a Deterministic Finite Automaton (DFA) with associated attributes. It provides methods for encoding and decoding state transitions, reading and writing to streams or files, and parsing tokens.

### `DFA_State`

#### Writing States

- **Method**: `std::ostream& Fast_Attribute_DFA::DFA_State::write(std::ostream& o)`
  - **Description**: Writes state information to an output stream `o`, including attribute priority, acceptance status, and encoded transitions.
  - **Returns**: Reference to the output stream.

#### Reading States

- **Method**: `std::istream& Fast_Attribute_DFA::DFA_State::read(std::istream& in)`
  - **Description**: Reads state information from an input stream `in`, including attribute priority, acceptance status, and encoded transitions.
  - **Returns**: Reference to the input stream.

### Writing DFA

- **Method**: `std::ostream& Fast_Attribute_DFA::write(std::ostream& o)`
  - **Description**: Writes the entire DFA (states, attributes, and regex) to an output stream `o`.
  - **Returns**: Reference to the output stream.

### Reading DFA

- **Method**: `std::istream& Fast_Attribute_DFA::read(std::istream& in)`
  - **Description**: Reads the entire DFA (states, attributes, and regex) from an input stream `in`.
  - **Returns**: Reference to the input stream.

### Loading and Writing Files

- **Method**: `void Fast_Attribute_DFA::load_file(const std::string filepath)`
  - **Description**: Loads DFA information from a binary file specified by `filepath`.

- **Method**: `void Fast_Attribute_DFA::write_to_file(const std::string filepath)`
  - **Description**: Writes DFA information to a binary file specified by `filepath`.

## Constructors

- **Constructor**: `Fast_Attribute_DFA(const std::string& regex, const std::deque<std::string>& attributes)`
  - **Description**: Constructs a `Fast_Attribute_DFA` object with a regular expression (`regex`) and a collection of attributes (`attributes`).

## Comparison Operator

- **Method**: `bool Fast_Attribute_DFA::operator==(const Fast_Attribute_DFA& o) const noexcept`
  - **Description**: Compares two `Fast_Attribute_DFA` objects for equality.

## Stream Operators

- **Function**: `std::ostream& operator<<(std::ostream& o, const Fast_Attribute_DFA& dfa)`
  - **Description**: Outputs information about the DFA states, including transitions.

## Accessors

- **Method**: `size_t Fast_Attribute_DFA::start_state() const noexcept`
  - **Description**: Returns the index of the start state.

- **Method**: `uint32_t Fast_Attribute_DFA::transition(char symbol, uint32_t current_state) const noexcept`
  - **Description**: Computes the state transition for a given symbol and current state.

- **Method**: `const std::vector<Fast_Attribute_DFA::DFA_State>& Fast_Attribute_DFA::get_states() const noexcept`
  - **Description**: Returns the collection of DFA states.

- **Method**: `const std::string& Fast_Attribute_DFA::get_attribute(const DFA_State& state) const noexcept`
  - **Description**: Returns the attribute associated with a DFA state.

## Parsing Tokens

- **Method**: `Fast_Attribute_DFA::ParseRecord Fast_Attribute_DFA::parse_token_simple(const std::string_view input_text, const size_t start)`
  - **Description**: Parses a token from the input text, returning the resulting state and end index.

- **Method**: `SimpleLexToken Fast_Attribute_DFA::parse_token_no_attribute(const std::string_view input_text, const size_t start)`
  - **Description**: Parses a token from the input text, returning a `SimpleLexToken` without an attribute.

- **Method**: `LexToken Fast_Attribute_DFA::parse_token(const std::string_view input_text, const size_t start)`
  - **Description**: Parses a token from the input text, returning a `LexToken` with an associated attribute.

## Additional Notes

- **Encoding and Decoding**: The class includes methods for encoding and decoding state transitions to optimize storage and retrieval.

- **File I/O**: The class provides methods for reading and writing the DFA to binary files.

- **Equality Operator**: The class includes an equality operator to compare two `Fast_Attribute_DFA` objects for equivalence.