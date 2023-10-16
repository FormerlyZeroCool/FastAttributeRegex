# Attribute Lexeme Document

## Overview

This document defines a set of regular expressions with associated attributes for lexing a source code file. Each regular expression is followed by an attribute name enclosed in parentheses.

## Regular Expressions and Attributes

### 1. Comment

- Regular Expression: `@comment://.*\n`
- Attribute: `comment`

This regular expression matches comments in the form `//...` and assigns the attribute `comment`.

### 2. Error

- Regular Expression: `@error:(00+\.)|([0-9]*\.[0-9]+00)|(00+)`
- Attribute: `error`

This regular expression identifies error patterns and assigns the attribute `error`.

### 3. Keyword

- Regular Expression: `@keyword:(asm)|(if)|(else)|(for)|(while)|(extern)`
- Attribute: `keyword`

This regular expression captures keywords like `asm`, `if`, `else`, `for`, `while`, and `extern` and assigns the attribute `keyword`.

### 4. Identifier (ID)

- Regular Expression: `@id:(_|[a-zA-Z])(_|[0-9a-zA-Z])*`
- Attribute: `id`

This regular expression matches identifiers, adhering to the specified naming conventions, and assigns the attribute `id`.

### 5. Number

- Regular Expression: `@number:([0-9]+)|([0-9]*\.[0-9]*)`
- Attribute: `number`

This regular expression identifies numeric literals, including both integers and floating-point numbers, and assigns the attribute `number`.

### 6. String

- Regular Expression: `@string:"[ -!#-}]*"`
- Attribute: `string`

This regular expression matches strings enclosed in double quotes and assigns the attribute `string`.

### 7. Left Parenthesis

- Regular Expression: `@lpar:\(`
- Attribute: `lpar`

This regular expression captures left parentheses `(` and assigns the attribute `left_parenthesis`.

### 8. Right Parenthesis

- Regular Expression: `@rpar:\)`
- Attribute: `rpar`

This regular expression identifies right parentheses `)` and assigns the attribute `right_parenthesis`.

### 9. Left Brace

- Regular Expression: `@lbrace:{`
- Attribute: `lbrace`

This regular expression matches left braces `{` and assigns the attribute `left_brace`.

### 10. Right Brace

- Regular Expression: `@rbrace:}`
- Attribute: `rbrace`

This regular expression captures right braces `}` and assigns the attribute `right_brace`.

### 11. Semicolon

- Regular Expression: `@semicolon:;`
- Attribute: `semicolon`

This regular expression identifies semicolons `;` and assigns the attribute `semicolon`.

### 12. Multi-Character Binary Operator

- Regular Expression: `@multi_char_bin_op:(==)|(>=)|(<=)`
- Attribute: `multi_char_bin_op`

This regular expression matches multi-character binary operators such as `==`, `>=`, and `<=` and assigns the attribute `multi_char_bin_op`.

### 13. Assignment Operator

- Regular Expression: `@assign_op:=`
- Attribute: `assign_op`

This regular expression captures the assignment operator `=` and assigns the attribute `assign_op`.

### 14. Unary Operator

- Regular Expression: `@un_op:\+\+`
- Attribute: `un_op`

This regular expression identifies unary operators like `++` and assigns the attribute `un_op`.

### 15. Binary Operator

- Regular Expression: `@bin_op:<|>|\+|-|/|\*`
- Attribute: `bin_op`

This regular expression matches binary operators such as `<`, `>`, `+`, `-`, `/`, and `*` and assigns the attribute `bin_op`.

### 16. Whitespace (Skip)

- Regular Expression: `@skip: |\t`
- Attribute: `skip`

This regular expression matches whitespace characters (space or tab) and assigns the attribute `skip`. These characters are typically ignored during parsing.

## Usage

This attribute lexeme document provides a set of regular expressions for identifying various elements in a source code file, along with their associated attributes. These expressions can be used as a basis for building a lexer to tokenize source code.

### Example Usage

```cpp
Fast_Attribute_DFA dfa = compile_regex(read_file("example.txt"));
std::string input_code = read_file("source_code.txt");
example_lexing_entire_string(dfa, input_code);
```

In this example, `example.txt` is a string containing the regular expressions and attributes defined in this document. It is compiled into a DFA, and the source code from the file "source_code.txt" is tokenized using the generated DFA. The resulting tokens are printed to the console.