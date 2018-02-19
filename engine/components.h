#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include <string>

class Systems;
class World;
class TextureCache;
class FileSystem;
class Duk;

void
AddSystems(Systems* systems, Duk* duk);

void
LoadWorld(
    FileSystem* fs, World* world, TextureCache* cache, const std::string& path);

#endif  // EUPHORIA_COMPONENTS_H
