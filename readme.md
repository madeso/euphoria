![euphoria logo](data/branding/euphoria-logo-1280.png)

[![windows](https://github.com/madeso/euphoria/workflows/windows/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Awindows)
[![linux](https://github.com/madeso/euphoria/workflows/linux/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Alinux)
[![Coverage Status](https://coveralls.io/repos/github/madeso/euphoria/badge.svg?branch=master)](https://coveralls.io/github/madeso/euphoria?branch=master)

Euphoria is a data-driven "game engine". It's in quotes because

1. the term "engine" is really vague.
2. it also contains other things that aren't related to game engine at all (like csv import and a non-realtime ray tracer)

Euphoria is under heavy development, not ready for prime time, and the final product will be focused on live programming/automatic refresh between computer and target devices (that include computers) with a feature set that lies between love2d (do whatever you like) and fps game creator(restrictive but quick prototyping).

The folder structure should follow the [pitchfork layout](https://github.com/vector-of-bool/pitchfork).

All names should follow `snake_case`, both for variables, members, functions and files. The only exception are C++ macros that should follow `SHOUTING_SNAKE_CASE` and C++ template arguments that should follow `PascalCase`. Convertion functions should either be named `con` is the convertion is obvious, otherwise the should be named `c_foobar` where `foobar` describes the conversion, casts are discouraged tothe point of being forbidden outside of the 'convertion functions'.
