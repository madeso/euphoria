#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

class World;
class TextureCache;
class FileSystem;
class DukRegistry;
class ObjectCreator;

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    TextureCache*      cache,
    DukRegistry*       reg,
    const std::string& path,
    ObjectCreator* creator);

#endif  // EUPHORIA_LOADWORLD_H
