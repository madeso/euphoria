#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

#include "core/sol_forward.h"

class World;
class FileSystem;
class DukRegistry;
class ObjectCreator;

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    DukRegistry*       reg,
    const std::string& path,
    ObjectCreator*     creator,
    sol::state*        ctx);

#endif  // EUPHORIA_LOADWORLD_H
