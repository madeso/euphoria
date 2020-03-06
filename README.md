# Euphoria

The earlier increment of euphoria can be found at my [spacehustler](https://github.com/madeso/spacehustler) project. The plan was to make a generic engine, but despite good intentions, that project barely lasted a full game.

So I forked my new, not finished but playable, 2d game to this repo instead to become the 'new' euphoria.

The following table lists how the sources are structured
 
| Folder          | Description|
| ---             | --- |
| `editor`        | the editor for creating games, can be seen as the "unity editor" for euphoria |
| `t3d`           | the world editor for games, can be seens as "hammer" for euphoria |
| `painter`       | a minimal image drawer, can be seen as "photoshop light" |
| `engine`        | the engine that runs the games, can be seen as "love2d" for euphoria |
| `musikmaskin`   | a minimal music creator (daw) |
| `samples`       | sample applications that test the libraries |
| `file_types`    | file definitions(gaf), for sharing between projects |
| `core`          | contains functionality that doesnt depend on other libraries |
| `minsynth`      | minimal software synt |
| `window`        | sdl wrapper |
| `render`        | a library for rendering 2d and 3d content in real-time |
| `gui`           | a system for rendering gui with the rendering library|
| `duk`           | a ducktape wrapper so scriping in the engine can be done |
| `tests`         | unit tests (for core) |
| `dist`          | various files that both the samples and the engine uses, intended for "distribution" hence the name |
| `scripts`       | scripts for automated building |
| `external`      | external libraries |
| `cmake_modules` | find scripts for CMake |

# Coordinate system

* 0,0 is lower left
* x-axis is positive right
* y-axis is positive up

