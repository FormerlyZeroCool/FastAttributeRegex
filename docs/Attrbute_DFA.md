# Attribute_DFA Documentation

## Overview

The `Attribute_DFA` class represents a Deterministic Finite Automaton (DFA) with associated attributes. It provides methods for manipulating and analyzing DFAs, including parsing tokens, minimizing the DFA, and converting it to a faster representation.

## Constructors

### `Attribute_DFA::Attribute_DFA(std::string&& regex, std::deque<std::string>&& attributes)`

- **Description**: Constructs an `Attribute_DFA` object with a regular expression (`regex`) and a set of attributes.
- **Parameters**:
  - `regex` (R-value reference to `std::string`): The regular expression associated with the DFA.
  - `attributes` (R-value reference to `std::deque<std::string>`): A collection of attributes.
- **Side Effects**: Initializes the DFA with the provided regular expression and attributes. It also computes attribute priorities based on their order.

## Public Member Functions

### `size_t Attribute_DFA::get_priority(std::string_view attribute)`

- **Description**: Retrieves the priority of a given attribute.
- **Parameters**:
  - `attribute` (`std::string_view`): The attribute whose priority is requested.
- **Returns**: The priority of the attribute.
- **Throws**: Throws a `std::string` exception if the attribute is not found.

### `std::deque<Attribute_DFA::DFA_State>& Attribute_DFA::get_states() noexcept`

- **Description**: Returns a reference to the collection of DFA states.
- **Returns**: A reference to a `std::deque` containing DFA states.

### `LexToken Attribute_DFA::parse_token(const std::string_view input_text, const size_t start)`

- **Description**: Parses a token from the input text.
- **Parameters**:
  - `input_text` (`std::string_view`): The input text to be parsed.
  - `start` (`size_t`): The starting index for parsing.
- **Returns**: A `LexToken` object representing the parsed token.

### `void Attribute_DFA::set_regex(const std::string& regex) noexcept`

- **Description**: Updates the regular expression associated with the DFA.
- **Parameters**:
  - `regex` (`const std::string&`): The new regular expression.

### `void Attribute_DFA::print()`

- **Description**: Prints information about the DFA states, including transitions.
- **Side Effects**: Outputs information to the console.

### `size_t Attribute_DFA::start_state() const noexcept`

- **Description**: Returns the index of the start state.
- **Returns**: The index of the start state.

### `size_t Attribute_DFA::transition(char symbol, size_t current_state) const noexcept`

- **Description**: Computes the state transition for a given symbol and current state.
- **Parameters**:
  - `symbol` (`char`): The input symbol.
  - `current_state` (`size_t`): The current state index.
- **Returns**: The index of the next state after the transition.

### `Attribute_DFA Attribute_DFA::minimize()`

- **Description**: Minimizes the DFA.
- **Returns**: A minimized `Attribute_DFA` object.

### `Attribute_DFA Attribute_DFA::remove_indistinguishible_states()`

- **Description**: Removes indistinguishable states from the DFA.
- **Returns**: An `Attribute_DFA` object with indistinguishable states removed.

### `std::vector<std::pair<size_t, std::list<size_t>>> Attribute_DFA::group_states_by_distiguishibility()`

- **Description**: Groups states by distinguishability.
- **Returns**: A vector of pairs containing a hash value and a list of state indices.

### `Fast_Attribute_DFA Attribute_DFA::to_fast_dfa() const noexcept`

- **Description**: Converts the DFA to a faster representation (`Fast_Attribute_DFA`).
- **Returns**: A `Fast_Attribute_DFA` object.

## Helper Functions

### `char encode_escape_symbol(const char c) const noexcept`

- **Description**: Encodes an escaped symbol.

### `char decode_escape_symbol(const char c) const noexcept`

- **Description**: Decodes an escaped symbol.

### `NFA_State* load_states(std::string_view regex, NFA_State* start, NFA_State* end, std::string_view current_attribute)`

- **Description**: Loads states based on a regular expression, starting and ending pointers, and a current attribute.

### `bool is_escaped_symbol(const unsigned char c) const noexcept`

- **Description**: Checks if a character is an escaped symbol.

### `bool is_escapable_symbol(const char c) const noexcept`

- **Description**: Checks if a character is escapable.

### `void trim_block(std::string_view& block, char opening = '(') const noexcept`

- **Description**: Trims a block of text, typically enclosed in parentheses.