#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

class World;
class FileSystem;
class DukRegistry;
class ObjectCreator;

namespace duk
{
  class Context;

  class Duk;
}

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    DukRegistry*       reg,
    const std::string& path,
    ObjectCreator*     creator,
    duk::Context*      ctx,
    duk::Duk*          duk);

#endif  // EUPHORIA_LOADWORLD_H
