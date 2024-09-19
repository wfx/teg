So you want to help the developent of TEG? cool!
___

# How to send changes

The easiest way to get your changes included is to fork the project, and let us
know about your changes with a pull request. If you don't like to have an
account on github, you still can clone our repository (via https), and push
your changes to a different service.

You also can use git format-patch to create a patch series. But we strongly
prefer to have a "real" git repository to deal with, since it is much easier to
work with than patches.

# First steps

If you want to join the development of the software, you need to install the
development packages of the used libraries. For Debian based environments, you
can use the list in the file `docker/debian`

When you have the requirements, you can run ``./build`` in the root directory.

.. warning:: This script always erases the build directory first.

This script

* creates the autotools build infrastructure
* creates a local build directory (``bd``), which is ignored by git
* configures this build directory

  * With autotools maintainer mode (so you don't have to manually re-run
    autoconf/automake/... whenever you touch the build configuration)
  * Turns compiler warnings into errors
  * installs the software into ``bd/DD``
* build the software an run the unit tests

Once the ``build`` script finished, you can "install" the software with
``make -C bd install``, and then run it with ``bd/DD/bin/tegclient``.

# Coding style

This code style describes how the code should look at the end. We know that at
the moment the current code does not look like it. When you change existing
code, or write new functionality, please try to follow this style.

## Whitespaces and braces

We use the [K&R-Style](https://en.wikipedia.org/wiki/Indentation_style#K&R) with tabs for block indentation, and spaces for
continuation indentation.

Braces are set unconditionally, even if there is only one statement in your
if/while/for block. See the famous "goto fail" bug why this is a good idea.

You can use ``scripts/style`` to format your sources according to the common
style. When you use an IDE, you probably can also set it up that it will
reformat on every save.

## General style rules

### Functions and code blocks

Limit the size of your functions. Every function spanning more than one page or
is deeply nested is hard to comprehend. You should split those functions into
smaller ones and give them appropriate names.

Try to write algebraic functions, which don't rely on global state. This kind
of function is much easier to test. Also when you use such a function, you can
be sure that it behaves always the same way when you give it the same
parameters.

### Comments

Good comments can make the code better understandable. But bad comments can make
everything worse. In case of doubt try to clean up your code and introduce
little helper functions. It is better to have easy to understand code than to
have good comments which explain bad code.

### General quality

Don't introduce new compiler warnings, they usually point to underlying
problems. This is especially true for C code, where the compiler can do very
bad things if it does not see a function declaration.

## Language

New code uses C++20. We don't try to support ancient compilers.

### Built in data types

Use the correct data type for each fact. When you have something which can be
expressed as boolean, use bool (use ``#include <stdbool.h>`` in C code to get
this type). If the thing can't be negative, use unsigned. If you need an integer
of a defined width use the types from ``<stdint.h> / <cstdint>``. When you
iterate through an array by index, use std::size_t (or unsigned for some legacy
code).

Prefer enumerations over integer or boolean types. Especially when they appear
in function calls, enumerations can make the situation easier to understand.

```C

  frobnicate(23, true, false);

  frobnicate(first_thing_from_left, with_foo, without_bar);
```
Use class enums in C++ code.

## Error handling

Use ``assert()`` to check conditions in the code which are pre and
postconditions (like that an output pointer is not NULL). Don't use assert on
functions with side effects (assert calls can be disabled by configuration).

```C

  // do

  const bool ok=frobnicate();
  assert(ok);

  // don't: the call to frobnicate can be eliminated in non-debug builds
  assert(frobnicate());
```

## Variables


### Const


Const is your friend. Use it wherever possible.

### Initialization

Try to avoid split variable definition and initialization. Especially don't put
variable definitions at the top of functions. The time when compilers needed
this is gone.

### Scope and variable reuse

Limit the scope of variables to the most inner block possible. This make the
code more readable and reduces the possibility to introduce errors.

**Good**:

```C++

  for(std::size_t i=0; i<(sizeof(array)/sizeof(*array)); i++) {
      // for C code
  }

  for(std::size_t i=0; i<std::extent<decltype(array)>::value; i++) {
      // for C++ code
  }
```

**Better**:

```C++

  for(auto const& value: array) {
      // when you don't need the array position you can just iterate over the
    // fields
  }
```

**Bad**:

```C++

  int i; // no initialization

  ...

  for(i=0; i<(sizeof(array)/sizeof(*array)); i++) { // signed loop variable
      ...
  }

  for(i=0; i<N_ARRAY_COUNT; i++) { // limit not calculated from the array
      ...
  }

  for(i=0; i<23; i++) { // magic number and variable recycling
      ...
  }
```

## Return values

Don't use ``TEG_STATUS`` for new code. This type is a dependency magnet, and
does all kind of harm in different parts of the code.

When your function can't return anything, then don't return anything. This rule
ensures that the calling function does not have to cope with error handling.
When you return a constant in this case, it is very likely that the error
handling in the calling code either does not exist, or is defect.

If the function only can fail or pass, use bool, and make your function sound
natural to this. Like ``can_start_game()`` or ``is_human_player()``.

##Objects in global space

Avoid global space. The code right now uses much of it, and this makes it very
painful to work with.

## Tests

New functionality has to be accompanied with unit tests.

## Documentation

Put [doxygen](https://en.wikipedia.org/wiki/Doxygen) comments into the header for newly added parts (should the day come,
and parts of teg wander into a library, the header are the parts which are
available, while the sources are not).

Write down what the reason for this function is, together with pre and post
conditions.

# Checklist for a new release

## Preparing the release

* Does it compile on the CI?
* Append all new contributors to /PEOPLE
* Fill out /CHANGELOG
  * use ``gitk »previous-release-tag« »current-release-branch«`` to review the
    changes since the last release
  * summary of the most important changes
  * reference issues,pull requests or git commit hashes when needed, especially
    for security related events

## Doing the release


Create an annotated tag in the form ``»num«.»num«.»num«``. Don't add any prefix
or suffix. The tag has to match the version of ``AC_INIT`` in ``/configure.ac``

## After the release

* Do a version bump in ``/configure.ac`` for the following bug fix release
* Merge the release branch to ``master`` (keep an eye on the Changelog and
  version, since they tend to get messed up during the merge)
