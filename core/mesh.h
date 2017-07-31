#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <vector>
#include <string>

#include "core/vec3.h"
#include "core/rgb.h"

enum class WrapMode {
  REPEAT, CLAMP_TO_EDGE, MIRROR_REPEAT
};

class MeshPart {
public:
  MeshPart();

  int material;
  std::vector<float> points;
  std::vector<unsigned int> faces;
  int facecount;

  void AddPoint(float x, float y, float z, float u, float v);
  void AddFace(unsigned int a, unsigned int b, unsigned int c);
};

class Material {
public:
  Material();

  Rgb diffuse;
  float alpha;
  std::string texture;
  WrapMode wraps;
  WrapMode wrapt;
};

class Mesh {
public:
  std::vector<Material> materials;
  std::vector<MeshPart> parts;
};

class MeshLoadResult {
public:
  Mesh mesh;
  std::string error;
};

MeshLoadResult LoadMesh(const std::string& path);

Mesh CreateCube(float size);
Mesh CreateSphere(float size, const std::string& texture);
Mesh CreateBox(float width, float height, float depth);

#endif // CORE_MESH_H
