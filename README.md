# IMP Interpreter

A small interpreter of the IMP programming language.


## Build

- `make all` to build interpreter.
- `make repl` to run repl.
- `make example` to interpret "example/example.imp".
- `make clean` to remove build folder.


## Dependencies

- [flex](https://github.com/westes/flex)
- [bison](https://www.gnu.org/software/bison)
- [readline](https://tiswww.case.edu/php/chet/readline/rltop.html)

## Resources

- [Formal Methods and Functional Programming (Course Webpage), ETHZ](https://infsec.ethz.ch/education/ss2025/fmfp.html)


## IMP

[Syntax](/res/syntax.ebnf)


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


**Variable `<var>`**

- `[a-zA-Z][A-Za-z0-9]*`


**Numeral `<num>`**

- `[0-9]+`
