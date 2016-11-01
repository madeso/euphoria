#ifndef SPACETYPER_GL_H
#define SPACETYPER_GL_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#endif  // SPACETYPER_GL_H
