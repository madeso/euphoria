# Euphoria libraries

## log
logging library

## assert
assert library, uses log

### core
contains core functionality like math and other things that doesn't require external libraries that can't be embedded

### file_types
file definitions(gaf), for sharing between projects

### gui
a system for rendering gui with the rendering library

### minsynth
a minimal software synth

### render
a library for rendering 2d and 3d content in real-time

### tests
testing libraries that are useful in unittests. should be replaced with the catchy library

### window
sdl and imgui wrapper

<pre>
┌───────┐
│ test  │
├───────┤
│       │
└───────┘
</pre>


```
┌───┬──┬──┐
│   │  │  │
│   ├──┼──┤
│   │  │  │
├───┴──┴──┤
│         │
└─────────┘


┌───────┐
│ test  │
├───────┤
│       │
└───────┘

```
