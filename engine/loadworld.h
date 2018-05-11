#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

class World;
class FileSystem;
class DukRegistry;
class ObjectCreator;
class Context;

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    DukRegistry*       reg,
    const std::string& path,
    ObjectCreator*     creator,
    Context*           ctx);

#endif  // EUPHORIA_LOADWORLD_H
