#ifndef EUPHORIA_COMPILEDMESH_H
#define EUPHORIA_COMPILEDMESH_H

#include <memory>

#include "core/mesh.h"
#include "render/buffer.h"

// one part of the mesh, single material
class CompiledMeshPart {
 public:
  Vbo data;
  Vao config;
  Ebo tris;
  int tri_count;
};

// a collection of parts making up a mesh
class CompiledMesh {
 public:
  std::vector<std::shared_ptr<CompiledMeshPart>> parts;

  void Render();
};

std::shared_ptr<CompiledMesh> CompileMesh(const Mesh& mesh);

#endif //EUPHORIA_COMPILEDMESH_H
