# Custom Language Compiler

A multi-phase compiler implementation written in C++ that translates source code through lexical analysis, syntax analysis, semantic analysis, and generates Three Address Code (TAC) as intermediate representation.

## Overview

This compiler follows the traditional compilation pipeline, processing input source files through distinct phases to produce optimized intermediate code. The implementation demonstrates fundamental compiler design principles and techniques used in modern programming language translators.

## Architecture

The compiler consists of four main phases:

### 1. Lexical Analysis (Scanner/Tokenizer)
- Reads source code from input text files
- Breaks down the source code into tokens
- Identifies keywords, operators, identifiers, literals, and delimiters
- Handles whitespace and comments
- Reports lexical errors

### 2. Syntax Analysis (Parser)
- Takes tokens from the lexical analyzer
- Constructs a parse tree
- Validates program structure against grammar rules
- Implements Recursive Descent Parser
- Reports syntax errors with meaningful messages

### 3. Code Generation
- Generates Three Address Code (TAC)
- Produces intermediate representation suitable for optimization
- Handles control flow structures
- Manages temporary variables and labels

## Features

- **Multi-phase compilation**: Clean separation of concerns across compilation phases
- **Error handling**: Comprehensive error reporting at each phase
- **Symbol table**: Efficient symbol management with scope handling
- **TAC generation**: Produces clean three-address intermediate code
- **File I/O**: Reads from text files, outputs generated Three Address Code

## Prerequisites

- C++ compiler (GCC, Clang, or MSVC)
- C++11 or higher standard support
- Standard C++ libraries

## Input File Format

The compiler expects source code in a specific format. Here's a sample input file structure:

```
// Sample program
Adadi my_func(Adadi para_m, Adadi para__m)
{
		num_3 := num_3 + 5 * 2 - 1 ::
}
```

## Output Format

The compiler generates Three Address Code with the following format:

```
t0 := 5 * 2
t1 := num_3 + t0
t2 := t1 - 1
num_3 := t2
```

## Error Handling

The compiler provides detailed error messages for:

- **Lexical errors**: Invalid characters, malformed tokens
- **Syntax errors**: Grammar violations, missing punctuation





