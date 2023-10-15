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
<br>
The above code defines three classes of attributes<br>
named "keyword" "id" and "number" respectively<br>
in the LexToken, which is returned by the parseToken method they will have a string_view containing that attribute<br>
as well as a priority corresponding to the attribute, where "keyword" will have priority 1, "id" priority 2, and "number" priority 3 and so on for other classes<br>
The LexToken will also contain the index in the source text the token was parsed from<br>
as well as the actual text of the parsed token<br>