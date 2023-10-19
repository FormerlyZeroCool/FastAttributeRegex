## Here is the documentation for the supported regex operators `?`, `*`, `+`, `[]`, and `|`:

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

These operators provide powerful ways to define patterns for matching strings in regular expressions. They allow for flexibility in specifying the structure of the text you want to search for. Remember that regex patterns can be combined to create complex search patterns.