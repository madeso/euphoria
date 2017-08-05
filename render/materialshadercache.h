
#ifndef EUPHORIA_MATERIALSHADERCACHE_H
#define EUPHORIA_MATERIALSHADERCACHE_H

#include "core/cache.h"
#include "render/materialshader.h"

class MaterialShaderCache : public Cache<std::string, MaterialShader, MaterialShaderCache>
{
 public:
  MaterialShaderCache(FileSystem* fs) : fs_(fs) {}

  std::shared_ptr<MaterialShader> Create(const std::string& path)
  {
    auto ret = std::make_shared<MaterialShader>();
    ret->Load(fs_, path);
    return ret;
  }

 private:
  FileSystem* fs_;
};

#endif //EUPHORIA_MATERIALSHADERCACHE_H
