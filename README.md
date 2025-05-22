# IMP Interpreter

A small interpreter of the IMP programming language, written in *C*, with *flex* for lexing, and *bison* for parsing.

Currently, the core functionality of IMP is implemented, additionally, the *local variable extension*, and the *procedure extension* are also implemented. 

There are some syntactic enhancements, such as the boolean constants `true` and `false`, omitting parenthesis around expressions and sequential composition, omitting the else block of an if statment, and the ability to add arbitrary many semicolons after each statement.


## Dependencies

- [flex](https://github.com/westes/flex) lexer generator
- [bison](https://www.gnu.org/software/bison) parser generator
- [readline](https://tiswww.case.edu/php/chet/readline/rltop.html) handling userinput for repl


## Resources

- [Formal Methods and Functional Programming (Course Webpage), ETHZ](https://infsec.ethz.ch/education/ss2025/fmfp.html)


## Build

- `make all` to build interpreter.
- `make repl` to run repl.
- `make example` to interpret "examples/example.imp".
- `make clean` to remove build folder.

All build artifacts are created in the build folder `./build`, including the imp binary (`./build/imp`).


## Usage

```
Usage: imp [ARGS]
  (no args)          start REPL
  -i <program.imp>   interpret program
  -a <program.imp>   print ast
  -h                 print this message
```

REPL:

```
IMP REPL (type IMP statements or commands starting with '%')
Commands:
  %quit               exit
  %run <program.imp>  interpret program
  %set <var> <val>    set variable
  %print [<var>]      print variable, or all variables
  %procedures         list declared procedures
  %help               show this message
```

## IMP

### Example

```
procedure factorial(n;r) begin
  if n <= 0 then
    r := 1;
  else
    m := n - 1;
    factorial(m;r);
    r := r * n;
  end;
end;

n := 5;
factorial(n;r);
```

### Syntax

 - [Syntax (Reference EBNF)](/res/syntax.ebnf) might not always accurately reflect all syntax rules. (See [parser.y](src/parser.y) for current implemented parsing).


**Statement `<stm>`**

Variable assignment:

- `<var> := <aexp>` any variable not assigned, has the value 0.

Local variable:

- `var <var> := <aexp> in <stm> end`

Control flow:

- `if <bexp> then <stm> else <stm> end`
- `while <bexp> do <stm> end`
- `(<stm>; <stm>)` sequential composition, the first statement runs before the second
- `skip`, nop

Procedures:

- `procedure <ident>(<var>, ... ; <var>, ... ) begin <stm> end` declaration, first argument list are value arguments (vars passed to procedure), second argument list are variable arguments (vars returned from procedure).
- `<ident>(<var>, ... ; <var>, ... )` call


**Expression**

Arithmetic Expression `<aexp>`:

- `<num>`
- `<var>`
- `(<aexp> + <aexp>)`
- `(<aexp> - <aexp>)`
- `(<aexp> + <aexp>)`

Boolean Expression `<bexp>`:

- `not <bexp>`
- `(<bexp> or <bexp>)`
- `(<bexp> and <bexp>)`
- `<aexp> = <aexp>`
- `<aexp> # <aexp>` not equals
- `<aexp> < <aexp>`
- `<aexp> <= <aexp>`
- `<aexp> > <aexp>`
- `<aexp> >= <aexp>`
- `true`
- `false`


**Variable `<var>`**

- `<ident>`


**Numeral `<num>`**

- `[0-9]+`

**Identifier `<ident>`**

- `[a-zA-Z][A-Za-z0-9]*`