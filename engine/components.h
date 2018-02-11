#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include <string>

class Systems;
class World;
class TextureCache;

void
AddSystems(Systems* systems);

void
LoadWorld(World* world, const std::string& path, TextureCache* cache);

#endif  // EUPHORIA_COMPONENTS_H
