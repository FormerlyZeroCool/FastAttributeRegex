# Regex.hpp Documentation

## Overview

The `Regex.hpp` header file provides utility functions for compiling regular expressions into deterministic finite automata (DFA) with attributes. It leverages the `Attribute_NFA` and `Attribute_DFA` classes to perform this transformation.

## Functions

### `compile_regex` (string version)

```cpp
inline Fast_Attribute_DFA compile_regex(std::string regex)
```

This function compiles a regular expression provided as a string into a Fast_Attribute_DFA.

- **Parameters**:
  - `regex`: A string containing the regular expression.

- **Returns**:
  - A `Fast_Attribute_DFA` object representing the compiled regular expression.

### `compile_regex` (string_view version)

```cpp
inline Fast_Attribute_DFA compile_regex(std::string_view regex)
```

This function compiles a regular expression provided as a string view into a Fast_Attribute_DFA.

- **Parameters**:
  - `regex`: A `std::string_view` containing the regular expression.

- **Returns**:
  - A `Fast_Attribute_DFA` object representing the compiled regular expression.

## Usage

The `Regex.hpp` file provides convenient functions for compiling regular expressions. Users can pass either a `std::string` or a `std::string_view` containing the regular expression. The functions internally utilize the `Attribute_NFA` class to generate the corresponding DFA.

Example usage:

```cpp
#include "Regex.hpp"

// ...

std::string regex_pattern = "(ab|cd)*ef";
Fast_Attribute_DFA compiled_dfa = compile_regex(regex_pattern);

// Alternatively, using string_view
std::string_view regex_view = "a(b|c)+d";
Fast_Attribute_DFA compiled_dfa_view = compile_regex(regex_view);
```

## Important Note

Before using the functions provided by `Regex.hpp`, ensure that the header files `Attribute_NFA.hpp` and `Attribute_DFA.hpp` are available and properly included in your project. The functions are defined inline for ease of use and should be linked with the rest of your code during compilation.