![euphoria logo](data/branding/euphoria-logo-1280.png)

[![windows](https://github.com/madeso/euphoria/workflows/windows/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Awindows)
[![linux](https://github.com/madeso/euphoria/workflows/linux/badge.svg?branch=master)](https://github.com/madeso/euphoria/actions?query=workflow%3Alinux)
[![deploy-online-content](https://github.com/madeso/euphoria/actions/workflows/deploy-online-content.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/deploy-online-content.yml)

[![run-style-checks](https://github.com/madeso/euphoria/actions/workflows/run-style-checks.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-style-checks.yml)
[![run-cppcheck](https://github.com/madeso/euphoria/actions/workflows/run-cppcheck.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-cppcheck.yml)
[![run-cpplint](https://github.com/madeso/euphoria/actions/workflows/run-cpplint.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-cpplint.yml)

[![run-coverage](https://github.com/madeso/euphoria/actions/workflows/run-coverage.yml/badge.svg)](https://github.com/madeso/euphoria/actions/workflows/run-coverage.yml)
[![codecov](https://codecov.io/gh/madeso/euphoria/branch/master/graph/badge.svg?token=V9K2H0U03G)](https://codecov.io/gh/madeso/euphoria) [![Coverage Status](https://coveralls.io/repos/github/madeso/euphoria/badge.svg?branch=master)](https://coveralls.io/github/madeso/euphoria?branch=master)

Euphoria is a data-driven "game engine". It's in quotes because

1. the term "engine" is really vague.
2. it also contains other things that aren't related to game engine at all (like csv import and a non-realtime ray tracer)

Euphoria is under heavy development, not ready for prime time, and the final product will be focused on live programming/automatic refresh between computer and target devices (that include computers) with a feature set that lies between love2d (do whatever you like) and fps game creator(restrictive but quick prototyping).


## Demos
* There is a [youtube playlist here](https://www.youtube.com/playlist?list=PLLZf3o2GDQ_iTb2AjfPWNMu0IPSetTOhx) that has small sample videos of some parts.

* Various [screenshots](data/screenshots.md) what the engine can generate.

* For each commit, theere are a few [html friendly things generated over here](https://i.madeso.me/euphoria/) such as doxygen, some color palettes and smaller tests for collision and debugging tools.


## About the code

The folder structure should follow the [pitchfork layout](https://github.com/vector-of-bool/pitchfork).

![project layout](./docs/euphoria.excalidraw.svg)

All names should follow `snake_case`, both for variables, members, functions and files. The only exception are C++ macros that should follow `SHOUTING_SNAKE_CASE` and structs & template arguments that should follow `PascalCase`.

## Naming guidelines
* Prefer to name function after what is returns, not the algorithm (`get_sorted` or `sort_inplace` vs `quicksort`).
* Prefer to name function after what is does, not how it does it (`add_item` vs `emplace_back_item`).
* Avoid general names, be specific but don't be too specific.