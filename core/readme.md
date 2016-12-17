This folder contains the "engine" part that doesn't use any libraries that can't
be "unittested". OpenGL is out of question, but certain parts of a protobuf
library might not be (printing to string for example).

Current things to add:
* plane, line
* collision stuff
* frustrum
* sphere, aabb

