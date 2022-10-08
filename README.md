## Overview
This is a simplified version of shenniger/dolorem to show the basic principle of
a dolorem staging system.

## Try it online
Try it here: https://shenniger.github.io/try-dolorem-c/

## Try it on your own computer

### Dependencies
* Mac OS/Linux (BSD might work)
* either clang in PATH or libtcc

### Compiling it
Just type `make`. You may need to specify the environment variable `CC`, especially if you installed libtcc in a non-standard directory.

If you want to use tcc compilation (much, much faster, but currently only works on Linux), use `make TCC=1`.

### First steps
There is some simple example code in this repository. To run it, type `./dolorem a.dlr`. (Depending on the system, you may need to add `.` to LD's search path. `export LD_LIBRARY_PATH=.:$LD_LIBRARY:PATH` should do it.)

#### Hello, world
Here's a dolorem-c program that prints "Hello, world!" during compilation:

```
(include "def.dlr")
(defun hello () void (puts "Hello, world!"))
(defmacro run (progn (hello) (make-cexp "" "" "" "")))
(run)
```

Put this into a file and then call dolorem on it.

The first lines includes the standard macros and headers from "def.dlr". The second line
defines a function called `hello` that calls `puts` to print the string. Now we need to
call `hello`. The easiest way is to create a macro `run` that calls `hello` and then evaluates
to nothing and to run that macro after.

#### Addition
Dolorem-c currently does not come with any kind of arithmetic abilities. Fortunately,
this is very easy to change. Suppose we want to create a macro `add` that generates the C
code for adding two numbers. We start with this (with a question mark instead of the return
value):

```
(defmacro add ?)
```

Since our macro will be multiple lines long, we add `progn`. Within that, we create two
variables, `left` and `right` to store the two lowered operands. Note that macro parameters
are always stored in an argument named `args`.

```
(defmacro add (progn
  (:= left (lower (car args)))
  (:= right (lower (car (cdr args))))
 ?))
```

We now have two `cexp`s. Their context, global, header attributes will simply needed to be
added together to reach the result and their expression attribute will be
`(left.expression)+(right.expression)`. To do this, we first call `add-cexp` to add everything of
the `right` to `left`. This function helpfully leaves `left.expression` untouched. We then
call `set-expression` to set the expression and return the changed `left`.

```
(defmacro add (progn
  (:= left (lower (car args)))
  (:= right (lower (car (cdr args))))
  (add-cexp left right)
  (set-expression left (print-to-mem "(%s) + (%s)" (get-expression left) (get-expression right)))
 left))
```

This is our finished `add` macro. We can use it like this: `(printf "1+1=%i\n" (add 1 1))`.
