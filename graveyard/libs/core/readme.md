# About the core library
The core library contains the "engine" part that doesn't use any libraries
that can't be "unittested". OpenGL is out of question, but certain parts
of a protobuf library might not be (printing to string for example).

Current things to add:
* plane, line
* collision stuff
* frustrum
* sphere, aabb


## Warning about points and vector classes
*Note to self:* If wanting to add points to the vector classes, considering
that a point class doesn't have a good representation of various
coordinate spaces, doesn't handle hiearchical systems well and apoint is
really a offset from some origo.

### Example time!

This is a simple code from [a while back](https://github.com/madeso/euphoria/blob/bec6a89991a93faff978e721b3c37eb419778eb9/core/path.cc#L207)

```cpp
c = point2f::FromOrigoTo((vec2f::FromOrigoTo(a) + vec2f::FromOrigoTo(b))*0.5f); // (1)
```

It's translating from the point representation, to a vector representation from
origo and back to a point representation. Using only vector representations
instead of points, we can express this easier:

```cpp
c = (a+b)*0.5f; // (2)
```
    
Another way to represent this mathematical operation would be something like

```cpp
c = a + (b-a) * 0.5f; // (3)
```

however that would be one extra operation, and hence "slower".

