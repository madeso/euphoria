# Euphoria

The earlier increment of euphoria can be found at my [spacehustler](https://github.com/madeso/spacehustler) project. The plan was to make a generic engine, but despite good intentions, that project barely lasted a full game.

So I forked my new, not finished but playable, 2d game to this repo instead to become the 'new' euphoria.

The following table lists how the sorces are structured
 
| Folder | Description|
| --- | --- |
| `scimed` | Scalable Image Editor, editor for editing scalable images |
| `core` | contains functionality that doesnt depend on other libraries |
| `render` | the library that uses opengl for rendering |
| `gui` | a 2d system for rendering gui |
| `proto` | protobuf files, for sharing between projects |
| `tests` | unit tests |
| `external` | external libraries |
| `cmake_modules` | find scripts for CMake |

