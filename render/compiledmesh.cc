#include "render/compiledmesh.h"
#include "render/shaderattribute3d.h"

std::shared_ptr<CompiledMesh> CompileMesh(const Mesh& mesh) {
  std::shared_ptr<CompiledMesh> ret { new CompiledMesh {} };

  for(const auto& part_src: mesh.parts) {
    std::shared_ptr<CompiledMeshPart> part { new CompiledMeshPart{} };

    Vao::Bind(&part->config);
    Vbo::Bind(&part->data);
    Ebo::Bind(&part->tris);

    part->data.SetData(part_src.points);
    part->config.BindVboData(attributes3d::Vertex(), 5 * sizeof(float), 0);
    part->config.BindVboData(attributes3d::TexCoord(), 5 * sizeof(float), 3 * sizeof(float));


    part->tris.SetData(part_src.faces);
    part->tri_count = part_src.facecount;

    Ebo::Bind(nullptr);
    Vbo::Bind(nullptr);
    Vao::Bind(nullptr);

    // todo: setup material and rendering stuff

    ret->parts.push_back(part);
  }

  return ret;
}

void CompiledMesh::Render() {
  for(const auto& part : parts) {
    Vao::Bind(&part->config);
    Ebo::Bind(&part->tris);
    part->tris.Draw(part->tri_count);
    Ebo::Bind(nullptr);
    Vao::Bind(nullptr);
  }
}
