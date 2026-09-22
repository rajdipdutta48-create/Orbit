# Orbit

> **Programming Beyond Earth** — a space-themed programming language and browser IDE built from scratch in C++.

Orbit is a custom interpreted programming language with its own lexer, recursive-descent parser, AST, interpreter, runtime values, arrays, functions, control flow, and a browser-based IDE.

## ✦ Features

### Language

* Variables with `dock`
* Output with `transmit`
* Input with `receive`
* Numbers and decimal/double values
* Strings and characters
* Boolean values
* Arithmetic: `+`, `-`, `*`, `/`, `%`
* Comparisons: `<`, `<=`, `>`, `>=`
* Equality: `==`, `!=`
* Logical operators: `!`, `&&`, `||`
* Assignment and chained assignment
* Conditional execution with `when` / `else`
* Loops with `orbiting`
* Custom multiline comments with `comet: ... burn`
* Nebula arrays
* Nested and multidimensional arrays
* Chained array indexing and indexed assignment
* Typed `receive()` for variables and array elements
* User-defined functions with `warp`
* Function parameters and arguments
* `return` statements
* Recursive function calls
* Function-local scope

## 🚀 Example

```orbit
comet:
A simple Orbit program
burn

dock message = "Hello Orbit";
dock value = 117 % 12;

transmit(message);
transmit(value);
```

Output:

```text
Hello Orbit
9
```

### Functions

```orbit
warp add(a, b) {
    return a + b;
}

dock result = add(10, 20);
transmit(result);
```

Output:

```text
30
```

### Recursion

```orbit
warp countdown(n) {
    when (n > 0) {
        transmit(n);
        return countdown(n - 1);
    }
}

countdown(5);
```

Output:

```text
5
4
3
2
1
```

### Multidimensional arrays

```orbit
nebula matrix = [
    [1.5, 2.5],
    [3.5, 4.5]
];

transmit(matrix[0][1]);

matrix[1][0] = 99.5;

transmit(matrix);
```

Output:

```text
2.5
[[1.5, 2.5], [99.5, 4.5]]
```

## 🛰️ Architecture

```text
Orbit Source Code
       │
       ▼
     Lexer
       │
       ▼
     Tokens
       │
       ▼
 Recursive-Descent Parser
       │
       ▼
       AST
       │
       ▼
   Interpreter
       │
       ▼
 Runtime Values / Output
```

### Browser IDE

```text
React + Vite
     │
     │ HTTP / JSON
     ▼
Express Server
     │
     ▼
Orbit Executable
     │
     ▼
Lexer → Parser → AST → Interpreter
```

The browser IDE provides:

* Code editor
* Line numbers
* Native text editing behavior
* Syntax highlighting
* Run/Launch button
* Program input area
* Mission Console output
* Orbit language documentation

## 📁 Project Structure

```text
Orbit/
├── include/
│   ├── expr.h
│   ├── interpreter.h
│   ├── lexer.h
│   ├── parser.h
│   └── token.h
│
├── src/
│   ├── expr.cpp
│   ├── interpreter.cpp
│   ├── lexer.cpp
│   ├── parser.cpp
│   └── token.cpp
│
├── client/
│   ├── src/
│   │   ├── App.jsx
│   │   ├── App.css
│   │   └── index.css
│   ├── package.json
│   └── ...
│
├── server/
│   └── server.js
│
├── orbit
└── README.md
```

## 🛠️ Run Locally

### 1. Build the Orbit interpreter

From the project root:

```bash
g++ -std=c++17 src/*.cpp -Iinclude -o orbit
```

### 2. Run an Orbit source file

```bash
./orbit program.orbit
```

### 3. Start the backend

```bash
cd server
node server.js
```

The backend runs the compiled Orbit executable for browser requests.

### 4. Start the frontend

In another terminal:

```bash
cd client
npm install
npm run dev
```

Then open the Vite development URL shown in the terminal.

## 📚 Language Reference

The browser IDE contains the full Orbit documentation, including:

* Variables
* Initialization
* Numbers and doubles
* Strings and characters
* Boolean values
* Comments
* Input/output
* Assignment
* Arithmetic and modulo
* Comparisons and logical operators
* Conditions and loops
* Nebula arrays
* 2D and multidimensional arrays
* Indexing and indexed assignment
* `warp` functions
* Parameters and arguments
* `return`
* Recursion
* Common mistakes
* Error guidance

## 🧠 Implementation Highlights

Orbit is implemented as a classic interpreter pipeline.

### Lexer

Converts source code into tokens and tracks source line numbers.

### Parser

Uses recursive descent with explicit operator precedence and builds an AST.

### AST

Separates expressions and statements into dedicated C++ node types such as:

```text
Binary
Assignment
IndexExpr
Call
FunctionStmt
ReturnStmt
```

### Interpreter

Evaluates expressions and executes statements using runtime `Value`s implemented with `std::variant`.

Runtime values currently include:

```text
double
bool
string
char
ArrayValue
FunctionValue
```

Functions use scoped environments so parameters and local variables remain isolated from the global scope.

## 🌌 Why Orbit?

Orbit is a systems-oriented project built to understand how programming languages work internally rather than treating a language runtime as a black box.

The project covers:

* Lexical analysis
* Syntax parsing
* AST construction
* Expression evaluation
* Statement execution
* Runtime environments
* Function calls
* Return propagation
* Array representation
* Browser-to-native execution

## 🔗 Repository

GitHub: https://github.com/rajdipdutta48-create/Orbit

## 📸 Screenshots

Add screenshots of the final browser IDE here after deployment.

Suggested screenshots:

1. Main Orbit IDE
2. Syntax-highlighted program
3. Documentation panel
4. Function/recursion example
5. Multidimensional array example

## 📌 Project Summary

**Orbit — Custom Programming Language & Browser IDE**

Built a space-themed interpreted programming language in C++ with a custom lexer, recursive-descent parser, AST, interpreter, scoped function runtime, arrays, multidimensional indexing, control flow, input/output, modulo arithmetic, recursion, and a React/Vite browser IDE backed by Express.
