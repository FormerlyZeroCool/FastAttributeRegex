# Source Code Documentation

## Overview

This source code provides utility functions and classes for text processing and lexing. It includes a class `Span` for representing a span of elements in a collection, along with structures `LexToken` and `SimpleLexToken` to represent tokens with associated attributes. Additionally, it defines a function `read_file` for reading the contents of a file into a string.

## Constants

### FNV_PRIME_32

- Type: `constexpr size_t`
- Value: 16777619

This constant represents the FNV prime value used in hashing algorithms.

### FNV_OFFSET_32

- Type: `constexpr size_t`
- Value: 2166136261U

This constant represents the FNV offset value used in hashing algorithms.

## Class: Span

### Description

`Span` is a template class designed to represent a span of elements in a collection. It provides convenient access to a subset of elements within the collection.

### Constructors

1. `Span(Collection& coll, size_t start)`: Initializes a `Span` with a given collection and starting index.

### Member Functions

- `operator[](size_t index) noexcept`: Provides access to the element at a specific index within the span.
- `get(size_t index) const noexcept`: Retrieves the element at a specific index within the span.
- `size() const noexcept`: Returns the length of the span.

## Struct: LexToken

### Description

`LexToken` is a structure representing a token with associated attributes. It includes an index, priority, attribute, and text.

### Members

- `index`: The index of the token.
- `priority`: The priority of the token.
- `attribute`: A `std::string_view` representing the attribute of the token.
- `text`: A `std::string_view` representing the text of the token.

### Constructor

- `LexToken(size_t index, size_t priority, std::string_view attribute, std::string_view text)`: Initializes a `LexToken` with the provided parameters.

### Overloaded Operator

- `operator<<`: Outputs the `LexToken` in the format `<attribute:priority->text>`.

## Struct: SimpleLexToken

### Description

`SimpleLexToken` is a simplified version of `LexToken` without an explicit attribute. It includes an index, priority, and text.

### Members

- `index`: The index of the token.
- `priority`: The priority of the token.
- `text`: A `std::string_view` representing the text of the token.

### Constructor

- `SimpleLexToken(size_t index, size_t priority, std::string_view text)`: Initializes a `SimpleLexToken` with the provided parameters.

### Overloaded Operator

- `operator<<`: Outputs the `SimpleLexToken` in the format `<priority->text>`.

## Function: read_file

### Description

`read_file` is a utility function that reads the contents of a file specified by its file path and returns the content as a string.

### Parameters

- `filePath`: A `std::string` representing the path to the file.

### Return Value

- Returns the content of the file as a `std::string`.

### Example Usage

```cpp
std::string content = read_file("example.txt");
```

In this example, the function `read_file` is used to read the contents of the file "example.txt" and store it in the `content` variable as a string.