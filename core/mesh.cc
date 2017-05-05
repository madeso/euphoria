#include "core/mesh.h"

#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/str.h"



MeshPart::MeshPart() : material(0) {}
void MeshPart::AddPoint(float x, float y, float z, float u, float v) {
  points.push_back(x);
  points.push_back(y);
  points.push_back(z);

  points.push_back(u);
  points.push_back(v);
}

void MeshPart::AddFace(int a, int b, int c) {
  faces.push_back(a);
  faces.push_back(b);
  faces.push_back(c);
}

Material::Material()
  : diffuse(Rgb::From(Color::White))
  , alpha(1.0f)
  , wraps(WrapMode::REPEAT)
  , wrapt(WrapMode::REPEAT)
{}


namespace {
  const unsigned int kAssimpFlags =
    aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType |
    aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_TransformUVCoords |
    aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials |
    aiProcess_PreTransformVertices | aiProcess_ImproveCacheLocality |
    aiProcess_FindDegenerates | aiProcess_JoinIdenticalVertices |
    aiProcess_ValidateDataStructure | aiProcess_GenSmoothNormals |
    aiProcess_FindInvalidData;

  WrapMode GetTextureWrappingMode(const int mode) {
    switch (mode) {
      case aiTextureMapMode_Wrap:
        return WrapMode::REPEAT;
      case aiTextureMapMode_Clamp:
        return WrapMode::CLAMP_TO_EDGE;
      case aiTextureMapMode_Decal:
        throw "Unsupported texture wrapping mode: decal";
      case aiTextureMapMode_Mirror:
        return WrapMode::MIRROR_REPEAT;
      default:
        throw "Unhandled texture wrapping mode";
    }
  }

  void AddMaterials(Mesh* ret, const aiScene* scene) {
    for (unsigned int material_id = 0; material_id < scene->mNumMaterials;
         ++material_id) {
      const aiMaterial* mat = scene->mMaterials[material_id];

      Material material;

      if (mat->GetTextureCount(aiTextureType_DIFFUSE) <= 0) {
        /*
        const std::string error = Str() << "Missing texture"; throw error;
        */
        /// @todo change to a better path, possible configurable or a auto
        /// generated one
        material.texture = "hazard.png";
      } else {
        aiString texture;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
        material.texture = texture.C_Str();
      }

      int u = 0;
      int v = 0;
      mat->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, 0), u);
      mat->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, 0), v);
      material.wraps = GetTextureWrappingMode(u);
      material.wrapt = GetTextureWrappingMode(v);
      ret->materials.push_back(material);
    }
  }

  void AddFaces(MeshPart* part, const aiMesh* mesh) {
    for (unsigned int face_id = 0; face_id < mesh->mNumFaces; ++face_id) {
      const aiFace& face = mesh->mFaces[face_id];
      part->AddFace(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }
  }

  void AddPoints(MeshPart* part, const aiMesh* mesh) {
    for (unsigned int index = 0; index < mesh->mNumVertices; ++index) {
      const aiVector3D& vertex = mesh->mVertices[index];
      float u = 0;
      float v = 0;
      if (mesh->HasTextureCoords(0)) {
        const aiVector3D uv = mesh->mTextureCoords[0][index];
        u = uv.x;
        v = uv.y;
      }
      part->AddPoint(vertex.x, vertex.y, vertex.z, u, v);
    }
  }

  MeshPart ConvertMesh(const aiMesh* mesh) {
    MeshPart part;

    part.material = mesh->mMaterialIndex;
    AddPoints(&part, mesh);
    AddFaces(&part, mesh);

    return part;
  }

  Mesh ConvertScene(const aiScene* scene) {
    Mesh ret;

    /** @todo add parsing of nodes to the mesh so we could
    dynamically animate some rotors, wings etc. for example
     */

    if (scene->HasMeshes()) {
      AddMaterials(&ret, scene);

      for (unsigned int meshid = 0; meshid < scene->mNumMeshes; ++meshid) {
        const aiMesh* mesh = scene->mMeshes[meshid];
        const MeshPart part = ConvertMesh(mesh);
        ret.parts.push_back(part);
      }
    }

    return ret;
  }

  // http://assimp.sourceforge.net/howtoBasicShapes.html
  Mesh LoadFromString(const std::string& nff, const std::string& format) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(
      nff.c_str(), nff.length() + 1, kAssimpFlags, format.c_str());
    if (!scene) {
      throw importer.GetErrorString();
    }
    return ConvertScene(scene);
  }

  const std::string kFormatNff = "nff";
  const std::string kFormatObj = "obj";

}  // namespace

MeshLoadResult LoadMesh(const std::string& path) {
  Assimp::Importer importer;
  MeshLoadResult res;

  const aiScene* scene = importer.ReadFile(path, kAssimpFlags);
  if (!scene) {
    res.error = importer.GetErrorString();
  }
  else {
    res.mesh = ConvertScene(scene);
  }
  return res;
}

Mesh CreateCube(float size) {
  std::ostringstream ss;
  ss << "shader texture.png" << std::endl << "hex 0 0 0 " << size;
  return LoadFromString(ss.str(), kFormatNff);
}

Mesh CreateSphere(float size, const std::string& texture) {
  std::ostringstream ss;
  ss << "shader " << texture << std::endl << "s 0 0 0 " << size;
  return LoadFromString(ss.str(), kFormatNff);
}

Mesh CreateBox(float width, float height, float depth) {
  const float x = width / 2;
  const float y = height / 2;
  const float z = depth / 2;
  std::ostringstream ss;
  ss << "v " << -x << " "
     << " " << -y << " " << -z << std::endl << "v " << -x << " "
     << " " << -y << " " << z << std::endl << "v " << -x << " "
     << " " << y << " " << -z << std::endl << "v " << -x << " "
     << " " << y << " " << z << std::endl << "v " << x << " "
     << " " << -y << " " << -z << std::endl << "v " << x << " "
     << " " << -y << " " << z << std::endl << "v " << x << " "
     << " " << y << " " << -z << std::endl << "v " << x << " "
     << " " << y << " " << z << std::endl << "" << std::endl << "vn 0 0 -1"
     << std::endl << "vn 0 0 1" << std::endl << "vn -1 0 0" << std::endl
     << "vn 1 0 0" << std::endl << "vn 0 1 0" << std::endl << "vn 0 -1 0"
     << std::endl << "vn 0 0 -1" << std::endl << "vn 0 -1 0" << std::endl
     << "vn -1 0 0" << std::endl << "vn 0 0 1" << std::endl << "vn 0 -1 0"
     << std::endl << "vn -1 0 0" << std::endl << "vn 0 0 -1" << std::endl
     << "vn 0 1 0" << std::endl << "vn -1 0 0" << std::endl << "vn 0 0 1"
     << std::endl << "vn 0 1 0" << std::endl << "vn -1 0 0" << std::endl
     << "vn 0 0 -1" << std::endl << "vn 0 -1 0" << std::endl << "vn 1 0 0"
     << std::endl << "vn 0 0 1" << std::endl << "vn 0 -1 0" << std::endl
     << "vn 1 0 0" << std::endl << "vn 0 0 -1" << std::endl << "vn 0 1 0"
     << std::endl << "vn 1 0 0" << std::endl << "vn 0 0 1" << std::endl
     << "vn 0 1 0" << std::endl << "vn 1 0 0" << std::endl << "" << std::endl
     << "vt 0 0" << std::endl << "vt 0 1" << std::endl << "vt 1 1" << std::endl
     << "vt 1 0" << std::endl << "" << std::endl
     << "f 3/1/13 7/2/25 5/3/19 1/4/7" << std::endl
     << "f 6/1/22 8/2/28 4/3/16 2/4/10" << std::endl
     << "f 2/1/12 4/2/18 3/3/15 1/4/9" << std::endl
     << "f 7/1/27 8/2/30 6/3/24 5/4/21" << std::endl
     << "f 4/1/17 8/2/29 7/3/26 3/4/14" << std::endl
     << "f 5/1/20 6/2/23 2/3/11 1/4/8" << std::endl;

  return LoadFromString(ss.str(), kFormatObj);
}
