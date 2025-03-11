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
- [ ] Semantic analysis *IN PROGRESS*
- [ ] Code generation
- [ ] Try to [compile in Windows 95](https://www.vogons.org/viewtopic.php?t=17324)
