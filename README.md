# FliteLang
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/IMSDcrueoft/FliteLang)

FliteLang is a subset of LoxFlux language (https://github.com/IMSDcrueoft/LoxFlux).

- **Design metrics**: Flite is just a toy language, and its syntax should be sufficient.

## List of FliteLang features

### Keywords
```
and or for break continue branch none class this typeof true false nil var fun return
```
### Syntax
``` ebnf
(* ===== Lexical Rules ===== *)
Letter          = "a".."z" | "A".."Z" | "_" ;
Digit           = "0".."9" ;
HexDigit        = Digit | "a".."f" | "A".."F" ;
BinDigit        = "0" | "1" ;

(* Operators from scanner.c token types *)
Operator        = "+" | "-" | "*" | "/" | "%" 
                | "==" | "!=" | "<" | "<=" | ">" | ">=" 
                | "<<" | ">>" | ">>>" | "&" | "|" | "^" | "~" 
                | "and" | "or" | "!" | "typeof" ;

(* Literals matching scanner.c implementation *)
StringLit       = "\"" (^"\n\"\\" | "\\" .)* "\"" ;         (* TOKEN_STRING *)
EscapeStringLit = "\"" (^"\n" | "\\" .)* "\"" ;             (* TOKEN_STRING_ESCAPE *)
NumberLit       = Digit+ ("." Digit+)? ([eE] [+-]? Digit+)?  (* TOKEN_NUMBER *)
                | "0" [bB] BinDigit+                         (* TOKEN_NUMBER_BIN *)
                | "0" [xX] HexDigit+ ;                      (* TOKEN_NUMBER_HEX *)

(* Identifiers and keywords from scanner.c's identifierType() *)
Identifier      = Letter (Letter | Digit)* ;
Keyword         = "class" | "fun" | "var" | "for" | "branch" 
                | "return" | "this" | "true" | "false" 
                | "nil" | "none" | "and" | "or" | "break" 
                | "continue" | "typeof" ;

(* Built-in modules from scanner.c's builtinType() *)
BuiltinModule   = "@math" | "@array" | "@string" | "@time" | "@sys" ;

(* ===== Syntax Rules ===== *)
Program         = { Declaration } EOF ;

(* Declaration types from compiler.c's declaration() *)
Declaration     = ClassDecl
                | FunDecl
                | VarDecl
                | Statement ;

(* Class structure from classDeclaration() *)
ClassDecl       = "class" Identifier "{" { Method } "}" ;
Method          = Identifier "(" [ Parameters ] ")" Block ;  (* TYPE_METHOD/TYPE_INITIALIZER *)

(* Function declarations from funDeclaration() *)
FunDecl         = "fun" Identifier "(" [ Parameters ] ")" Block ;

(* Variable declaration from varDeclaration() *)
VarDecl         = "var" Identifier ("=" Expression)? ("," Identifier ("=" Expression)?)* ";" ;

(* Statement types from compiler.c's statement() *)
Statement       = ExprStmt
                | ForStmt
                | BranchStmt     (* branchStatement() *)
                | ReturnStmt
                | Block
                | BreakStmt
                | ContinueStmt ;

(* Branch statement implementation from branchStatement() *)
BranchStmt      = "branch" "{" { Case } [ DefaultCase ] "}" ;
Case            = Expression ":" Statement ;                 (* branchCaseStatement() *)
DefaultCase     = "none" ":" Statement ;                     (* none case handling *)

(* Operator precedence levels from parsePrecedence() *)
Expression      = Assignment ;
Assignment      = Identifier "=" Assignment
                | LogicOr ;         (* PREC_ASSIGNMENT *)
LogicOr         = LogicAnd ( "or" LogicAnd )* ;  (* PREC_OR *)
LogicAnd        = Equality ( "and" Equality )* ; (* PREC_AND *)
Equality        = Comparison ( ("==" | "!=") Comparison )* ; (* PREC_EQUALITY *)
Comparison      = Term ( ("<" | "<=" | ">" | ">=") Term )* ;(* PREC_COMPARISON *)
Term            = Factor ( ("+" | "-") Factor )* ;           (* PREC_TERM *)
Factor          = Unary ( ("*" | "/" | "%") Unary )* ;       (* PREC_FACTOR *)

(* Postfix rules from dot(), call(), subscript() *)
Postfix         = Primary ( "(" [ Arguments ] ")"           (* OP_CALL/OP_INVOKE *)
                          | "." Identifier                  (* OP_GET_PROPERTY *)
                          | "[" Expression "]" )* ;         (* OP_GET_SUBSCRIPT *)

(* Primary expressions from compiler.c's primary rules *)
Primary         = "true" | "false" | "nil" | BuiltinModule  (* builtinLiteral() *)
                | NumberLit | StringLit | EscapeStringLit   (* number()/string() *)
                | "this"                                    (* this_() *)
                | "(" Expression ")"                        (* grouping() *)
                | "[" [ Expression ( "," Expression )* ] "]"(* arrayLiteral() *)
                | "{" [ (Identifier | StringLit) ":" Expression 
                      ( "," (Identifier | StringLit) ":" Expression )* ] "}" (* objectLiteral() *)
                | Identifier                                (* variable() *)
```

### Comment

- **Line & Block comment**: Using `//` or `/* */`.

---

### GC

- **Detached static objects and dynamic objects**: Static objects such as strings/functions, they don't usually bloat very much, so I think it's a viable option not to recycle them.

---

### Built-in Modules

There are some namespace objects that start with `'@'` available, and since they are not in the global scope, the initial state of the global scope is a "completely clean" state. 

#### `@math` `@array` `@string` `@sys`

The `@math` module provides a comprehensive set of mathematical functions and utilities, implemented as native bindings for efficiency and ease of use. These functions are accessible globally and can be used directly in scripts or applications.

- **Basic Arithmetic Functions**:
  - `max`: Returns the maximum value among the provided arguments.
  - `min`: Returns the minimum value among the provided arguments.
  - `abs`: Computes the absolute value of a number.
  - `floor`: Rounds a number down to the nearest integer.
  - `ceil`: Rounds a number up to the nearest integer.
  - `round`: Rounds a number to the nearest integer.

- **Exponential and Logarithmic Functions**:
  - `pow`: Computes the power of a number (x^y).
  - `sqrt`: Computes the square root of a number.
  - `exp`: Computes the exponential function (e^x).
  - `log`: Computes the natural logarithm (ln) of a number.
  - `log2`: Computes the base-2 logarithm of a number.
  - `log10`: Computes the base-10 logarithm of a number.

- **Trigonometric Functions**:
  - `sin`: Computes the sine of an angle (in radians).
  - `cos`: Computes the cosine of an angle (in radians).
  - `tan`: Computes the tangent of an angle (in radians).
  - `asin`: Computes the arcsine (inverse sine) of a number.
  - `acos`: Computes the arccosine (inverse cosine) of a number.
  - `atan`: Computes the arctangent (inverse tangent) of a number.

- **Random Number Generation**:
  - `random`: Generates a pseudo-random number using the xorshift128+ algorithm.
  - `seed`: Initializes the random number generator with a specific seed value.

- **Special Value Checks**:
  - `isNaN`: Checks if a value is NaN (Not a Number).
  - `isFinite`: Checks if a value is finite (not infinite or NaN).

These functions are designed to provide robust mathematical capabilities while maintaining high performance through native implementation. They are ideal for scientific computations, game development, simulations, and other domains requiring precise numerical operations.

---

The `@array` module provides robust support for working with arrays, enabling efficient manipulation of collections of data. These utilities allow developers to create and manage arrays of various types with fine-grained control.

- **Array Methods**:
  - `resize`: Resizes an existing array to a new length. If the new length is larger, additional elements are initialized to zero (or the default value for the type). If smaller, excess elements are discarded.
  - `length`: Returns the current number of elements in the array.
  - `pop`: Removes and returns the last element of the array. If the array is empty, it may return nil or throw an error, depending on configuration.
  - `push`: Appends one or more elements to the end of the array.

These utilities are invaluable for working with structured data, especially in performance-critical applications or environments where memory usage must be tightly controlled. They enable developers to manage arrays explicitly and efficiently.

---

The `@string` module provides advanced string manipulation capabilities, supporting both basic operations and high-performance string building. It handles ASCII and UTF-8 encodings with memory-efficient strategies, ideal for text processing tasks.

- **String Methods**:
  - `length`: Returns the byte length of a string.  
  - `utf8Len`: Returns the character count for UTF-8 strings (ignoring byte-level details). e.g.,`@string.utf8Len("αβγ")` → `3`
  - `charAt`: Retrieves an ASCII character by byte position.  
  - `utf8At`: Retrieves a UTF-8 character by logical character position. e.g.,`@string.utf8At("αβγ", 1)` → `"β"`

This module balances performance and safety for both simple text tasks and large-scale string processing.

---

The `@sys` module offers low-level system utilities, primarily focused on memory management and garbage collection. These functions provide insights into the runtime environment and allow fine-grained control over resource allocation.

- **Log**
  - `log`: Allows for multiple inputs and automatically expands the contents of the array and prints (but not recursively).

- **Garbage Collection**:
  - `gc`: Triggers a full garbage collection cycle.
  - `gcNext`: Configure the heap memory usage to be used for the next GC trigger.
  - `gcBegin`: Configure the limits of the initial GC.

- **Memory Statistics**:
  - `allocated`: Returns the total number of bytes currently allocated in the dynamic memory pool(includes built-in objects and deduplication pools).
  - `static`: Returns the total number of bytes allocated for static objects(e.g., strings, functions).

These utilities are invaluable for monitoring and optimizing memory usage, especially in long-running applications or environments with limited resources. They enable developers to manage memory explicitly and diagnose potential memory leaks or inefficiencies.

---

### REPL

- **Support for line break input**: Use `\` for multi-line input in REPL.
- **Commands**:
  - `/help` : Print help information.
  - `/exit` : Exit the REPL.
  - `/clear`: Clean the console.
  - `/eval` : Load file and Run.

## Licenses
The project **FliteLang** is based on `MIT`.
  - Copyright (c) 2025 IMSDCrueoft
  - License: `MIT`