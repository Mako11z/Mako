# My Language Specification

## Table of Contents

1. [Introduction](#introduction)
2. [Language Features](#language-features)
3. [Syntax Rules](#syntax-rules)
4. [Example Programs](#example-programs)
5. [Keywords and Operators](#keywords-and-operators)

## Introduction

## Language Features

Describe the main features of your language, such as:

- Variable declarations
  - Syntax: [Data Type] [Variable Name] = [Value];
- Arithmetic operations
  - Multiply '\*'
  - Divide '/'
  - Add '+'
  - Subtract '-'
  - Module '%'
- Print statements
  - Syntax: print()
- Data types
  - string (sequence of characters)
  - float
  - int
  - char (single character)
  - bool (true or false)
- Expressions and statements
  - Operations and assignments

## Syntax Rules

<variable_declaration> ::= <data_type> <identifier> "=" <expression> ";"
<data_types> : "int" | "float" | "string" | "char" | "bool"
<identifier> : Must start with a character (upper case or lower) and can be followed by special characters
<literal> : <integer_literal> | <float_literal> | <string_literal> | <char_literal> | <bool_literal>
<integer_literal> : [0-9]
<float_literal> : [0-9] "." [0-9]
<string_literal> : " a-zA-Z0-9... "
<char_literal> : ' a-zA-Z0-9 '
<bool_literal> : "true" | "false"

## Keywords

- int
- string
- float
- char
- bool
- print
- for
- if
- else

## Operators

- '+' (addition)
- '-' (subtraction)
- '\*' (multiplication)
- '/' (division)
- '%' (module)
- '=' (assignment)

## Punctuation

- ';'
- '(' and ')'
- '{' and '}'
