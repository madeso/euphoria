![euphoria logo](data/branding/euphoria-logo-1280.png)

[![windows](https://github.com/madeso/euphoria/workflows/windows/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Awindows)
[![linux](https://github.com/madeso/euphoria/workflows/linux/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Alinux)
[![run-style-checks](https://github.com/madeso/euphoria/actions/workflows/run-style-checks.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-style-checks.yml)
[![deploy-online-content](https://github.com/madeso/euphoria/actions/workflows/deploy-online-content.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/deploy-online-content.yml)

[![run-coverage](https://github.com/madeso/euphoria/actions/workflows/run-coverage.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-coverage.yml)
[![codecov](https://codecov.io/gh/madeso/euphoria/branch/master/graph/badge.svg?token=V9K2H0U03G)](https://codecov.io/gh/madeso/euphoria)

Euphoria is a data-driven "game engine". It's in quotes because

1. the term "engine" is really vague.
2. it also contains other things that aren't related to game engine at all (like csv import and a non-realtime ray tracer)

Euphoria is under heavy development, not ready for prime time, and the final product will be focused on live programming/automatic refresh between computer and target devices (that include computers) with a feature set that lies between love2d (do whatever you like) and fps game creator(restrictive but quick prototyping).

The folder structure should follow the [pitchfork layout](https://github.com/vector-of-bool/pitchfork).

All names should follow `snake_case`, both for variables, members, functions and files. The only exception are C++ macros that should follow `SHOUTING_SNAKE_CASE` and structs & template arguments that should follow `PascalCase`. Convertion functions should either be named `con` if the convertion is obvious, otherwise it should be named `c_foobar` where `foobar` describes the conversion, casts are discouraged to the point of being forbidden outside of the 'convertion functions'.

