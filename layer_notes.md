Current system:
- You can override macros.
- Advantage: flexible, relatively low overhead
- Disadvantage: verbose, need to make `lower` a macro which does not make sense

Build higher-level layer system on top of macro overriding.

Something like:

```
(new-layer funoverload
  (defun (progn
    (:= name (car args))
    (:= n-args (count-len (car (cdr args))))
    (hashmap-put overloaded-fun (expect-ident name) "")
    (val-set-string name (print-to-mem "%s__fo_%i" (expect-ident name) n-args))
   (fallback args)))
  (lower (progn
    (cond (val-is-list args) (progn
      (var dummy char*)
      (cond (not (hashmap-get overloaded-fun (expect-ident (car args)) (ptr-to dummy)))
        (val-set-string (car args) (print-to-mem "%s__fo_%i" (expect-ident (car args)) (count-len (cdr args)))))))
   (fallback args))))
```

The shortened form can override a number of macros at once. Each of the functions
gets `args` and can `(fallback …)` to give control to the layer underneath it.

Notes:
+ Any function that calls `(fallback args)` last (and always calls it), will look
  like a macro from lisp-variations (and thus be primarily syntactic sugar).
+ Any function that calls `(fallback args)` first (and always calls it), will be a
  C-level transformation.
+ Only functions that only sometimes call `(fallback args)` are actually additions
  to the compiler (and thus macros in the dolorem sense).
