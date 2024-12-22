# Grammar

Grammar is a C89 implementation of an LALR parsing table generator.

It's used in the 1 bootstrap compiler.

It can translate a grammar written in text and produce a function that can be used at any point to very quickly parse an input stream of tokens and generate a trivially walkable abstract syntax tree (AST).
