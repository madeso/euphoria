# Euphoria

[![Build Status](https://travis-ci.org/madeso/euphoria.svg?branch=master)](https://travis-ci.org/madeso/euphoria)
[![Build status](https://ci.appveyor.com/api/projects/status/vmq6ojb3xx01pwhk/branch/master?svg=true)](https://ci.appveyor.com/project/madeso/euphoria/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/madeso/euphoria/badge.svg?branch=master)](https://coveralls.io/github/madeso/euphoria?branch=master)

Euphoria is a data-driven "game engine". It's in quotes because

1. the term "engine" is really vague.
2. it also contains other things that aren't related to game engine at all (like csv import and a non-realtime ray tracer)

Euphoria is under heavy development, not ready for prime time, and the final product will be focused on live programming/automatic refresh between computer and target devices (that include computers) with a feature set that lies between love2d (do whatever you like) and fps game creator(restrictive but quick prototyping).

The following table lists how the source is structured:
 
| Folder          | Description|
| ---             | --- |
| `editor`        | the editor for creating games, can be seen as the "unity editor" for euphoria |
| `engine`        | the engine that runs the games, can be seen as "love2d" for euphoria |
| `t3d`           | the world editor for games, can be seens as "hammer" for euphoria |
| `painter`       | a minimal image drawer, can be seen as "photoshop light" |
| `musikmaskin`   | a minimal music creator (daw) |
| `samples`       | sample applications that test the libraries |
| `file_types`    | file definitions(gaf), for sharing between projects |
| `core`          | contains core functionality like math and other things that doesn't require external so/dlls|
| `minsynth`      | minimal software synt |
| `window`        | sdl and imgui wrapper |
| `render`        | a library for rendering 2d and 3d content in real-time |
| `gui`           | a system for rendering gui with the rendering library|
| `duk`           | a ducktape wrapper so scriping in the engine can be done |
| `tests`         | unit tests (for core) |
| `dist`          | various files that both the samples and the engine uses, intended for "distribution" hence the name |
| `scripts`       | scripts for automated building |
| `external`      | external libraries |
| `cmake_modules` | find scripts for CMake |

## Coordinate system

Euphoria works accordiing to the following coordinate system

* 0,0 is lower left
* x-axis is positive right
* y-axis is positive up

