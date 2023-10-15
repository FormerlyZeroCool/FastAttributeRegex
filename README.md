# FastAttributeRegex

A small library that implements attributable Unix style regex with deterministic finite automata<br>
<br>
Designed to be used to create dynamic Lexicographic analyzers that are fast, and easily integrated with C++ programs<br>
<br>
Example attribute regex:

```
@keyword:(asm)|(if)|(else)|(for)|(while)|(extern)
@id:(_|[a-zA-Z])(_|[0-9a-zA-Z])*
@number:([0-9]+)|([0-9]*\.[0-9]*)
```