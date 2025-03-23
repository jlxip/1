# The 1 System

## 1 Bootstrap Compiler

This is a compiler written in 100% portable C89. It translates 1 into C89.

Even though this compiles perfectly fine with `-Werror -Wall -Wextra -pedantic`,
you probably want to disable warnings when using an old compiler, in case of bugs.

## Status
- [x] Data structures
- [x] Lexer
- [x] LALR parser generator
- [x] Lexer (second thought)
- [x] Parser
- [ ] Full semantic analysis *IN PROGRESS*
- [ ] Free memory
- [ ] Compile-time interpreter
- [ ] Code generation
