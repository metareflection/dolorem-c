# Dolorem-C

A dolorem-c implementation does not call `eval` on a program; it calls `lower`. `lower` lowers a given `val` (an s-expression) into a `cexp` (a C expression).

## `cexp`
A `cexp` is an expression in C code. Because of C's syntax, which does not allow very complex expressions, we cannot simply represent it using one string. Instead, it consists of the following three strings:
- `expression`, which is the expression itself, e.g. `a+b`. It never ends in a semicolon. This can later be inserted into a statement.
- `context`, which is a list of statements that should precede whatever statement `expression` will be inserted into, e.g. `int a;`. If it is non-empty, this always ends in a closing curly parenthesis or a semicolon.
- `global`, which is a list of statements on global scope that should precede whatever function this `cexp` will be inserted into.
- `header`, which is a list of headers on global scope that need to be added to the environment after `cexp` has been compiled, e.g. `#include <stdio.h>` for a `cexp` that contains `printf`.

The following shows how `lower` and some other macros translate from dolorem-c into this representation. I hope these examples are helpful to understand precisely how this allows any dolorem-c expression to be represented in C.

In the following, I will write down `cexp`s as tuples of strings in square braces separated by vertical lines, e.g. `[a+sqrt(b)|||#include <math.h>]` for a `cexp` with `expression="a+sqrt(b)"`, an empty `context` and `global="#include <math.h>"`.

TODO: Note that, unlike Dolorem's `rtv`s, `cexp`s do not contain any frontend type information. Given that dolorem-c is more of a pedagogical language, does it make sense to leave type checking entirely to the C compiler? This would clearly make it less of a “serious language”, but reduce frontend complexity by a lot.

## `lower` on a number or a string
When called on a number (e.g. `lower 1`) or a string literal (e.g. `lower "hello"`), lower returns that number or string as `expression` with empty local and global context.

## `lower` on a function call
`lower (fun a b ...)` evaluates to `[fun(a.expression, b.expression, ...)|a.context b.context ...|a.global b.global ...|a.header b.header ...]`.

## `lower` on a macro
Like in Dolorem, when dolorem-c encounters a macro call, it does not lower it to C, but instead calls the macro and returns its result.

Some macros:

### Arithmetic operators
`lower (+ x y)` evaluates to `[(x.expression) + (y.expression)|x.context b.context|a.global b.global|a.header b.header]`.

### progn
`lower (progn a b c)` evaluates to `[c.expression|a.context a.expression; b.context b.expression; c.context|a.global b.global|a.header b.header]`.

### cond
`lower (cond c a)` evaluates to `[|c.context if(c.expression) { a.context a.expression; }|c.global a.global|a.header b.header]`.

### if
`lower (if c a b)` evaluates to `[temp|c.context int temp; if(c.expression) { a.context temp = a.expression; } else { b.context temp = b.expression; }|c.global a.global b.global|c.header a.header b.header]`.

[Note that this only works on ints because the type of the temp variable is unknown. The real implementation would either need a type system or use typeof() in the C code.]

### function
`defun` is split up into two parts: `function` prepares the C code and `compile` compiles and links it.

As a result, `lower (function name ((type1 arg1) ...) returntype body)` evaluates to `[||body.global returntype name(type1 arg1, ...) { body.context return body.expression; }|body.header returntype name(type1 arg1, ...);]` (or without the return statement for functions without return value).

### funproto
Same as function except it does not have a body and only evaluates to a header.

`lower (funproto name ((type1 arg1) ...) returntype)` evaluates to `[|||returntype name(type1 arg1, ...);]` (or without the return statement for functions without return value).

TODO: Later, we might want to add this to def.dlr. It does not need to be in the core language.

### compile
`lower (compile x)` is a special case: As the only macro in this list, it has a side effect and returns an empty cexp.

It lowers x, then takes (lower x).global, passes it to a C compiler and links in the result so that it is then available to be used as a macro or in macros.
x.header is being added to the global header list.

### mark-as-macro
`lower (mark-as-macro x)` is the only other special case. `mark-as-macro` takes a function name and adds it to the global macro list. It expects this macro to already be compiled at this point.

### TODO: quote

## Examples
```
> lower 3
> > [3|||]
> lower "hello, c"
> > ["hello, c"|||]
> lower (+ 3 4)
> > [3+4|||]
> lower (cond (> 3 4) (puts "test"))
> > [|if(3 > 4) { puts("test"); }||]
> lower (progn (cond (> 3 4) (puts "test")) 0)
> > [0|if(3 > 4) { puts("test"); }||]
> lower (function test () int (progn (cond (> 3 4) (puts "test")) 0))
> > [||int test() { if(3 > 4) { puts("test"); } return 0; }|int test();]
> compile (lower (function test () int (progn (cond (> 3 4) (puts "test")) 0)))
> > [|||] the symbol test is compiled and added to the environment and “int test();” is added to the global header list
```

### Hello, world!
```
(compile (funproto puts ((char* a)) void))
(compile (funproto make-cexp ((void* a) (void* before) (void* global) (void* header)) void*))
(compile (function hello () void (puts "hello, world!")))
```
This creates a function `hello`.

This runs it at compile-time (using the macro trick also used in Dolorem):

```
(compile (funproto make-cexp ((void* a) (void* before) (void* global) (void* header)) void*))
(compile (function run ((void* a)) void* (progn
  (hello)
  (make-cexp "" "" "" ""))))
(mark-as-macro run)
(run)
```

### Some useful macros
#### defun
#### defmacro

## Helper functions
### make-cexp
```
struct cexp *make_cexp(const char *a, const char *before, const char *global, const char* header);
```
Constructs a `cexp` from its four parts.

### add-cexp
```
void add_cexp(struct cexp **orig, struct cexp *add);
```
Adds context, global, header of `add` to `orig`. Leaves `orig.expression` unchanged.
