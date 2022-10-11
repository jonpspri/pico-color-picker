# Pico Color Picker Standards and Practices

## Types

* `struct`s should be `typedef`ed with a suffix of `_t`
* Magic Numbers are defined in `pcp.c`.  `assert` the magic number after casting from a `void *`.
  Trust me, it will save a lot of time later.

## Struct members
* Callbacks should have a `_cb` suffix (except for the context_callback_table).
* Put `void *data` as the _last member_ if "subclass" data is to be included.

## Functions

* Prefix extern functions (default) and header inline functions with the module name
* Prefix static functions with `s_` as a (short) reminder
* Callbacks should end in `_callback` and ideally be declared static.
* Arrange callbacks together at the start of the module for ease of reference.

## Arrays
* Pre-allocated arrays are _plural_
* Strive to use a compiler constant in the size definition of any array.

## Cross-object access
* If it feels icky it probably is.  Compromise on `static inline` getters and setters in 
  a header file if you feel like you're directly accessing something you shouldn't.  Having
  the lightweith abstraction will support later refactoring.
