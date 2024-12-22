# 1 Bootstrap Compiler

This is a compiler written in 100% portable C89. It translates 1 into C89.

Even though this compiles perfectly fine with `-Werror -Wall -Wextra -pedantic`,
you probably want to disable warnings when using an old compiler, in case of bugs.

## Status
- [x] Data structures
- [x] Lexer
- [ ] LALR parser generator [*IN PROGRESS*](src/parser/easy/README.md).
- [ ] Lexer (second thought)
- [ ] Parser
- [ ] Productions (semantic analysis)
- [ ] Code generation
