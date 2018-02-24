#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

class World;
class TextureCache;
class FileSystem;

void
LoadWorld(
    FileSystem* fs, World* world, TextureCache* cache, const std::string& path);

#endif  // EUPHORIA_LOADWORLD_H
