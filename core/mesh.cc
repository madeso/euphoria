#include "core/mesh.h"

#include <sstream>

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/str.h"
#include "core/texturetypes.h"


MeshPart::MeshPart()
    : material(0)
    , facecount(0)
{
}

void
MeshPart::AddPoint(float x, float y, float z, float nx, float ny, float nz,
                   float u, float v)
{
  points.push_back(x);
  points.push_back(y);
  points.push_back(z);

  points.push_back(nx);
  points.push_back(ny);
  points.push_back(nz);

  points.push_back(u);
  points.push_back(v);
}

void
MeshPart::AddFace(unsigned int a, unsigned int b, unsigned int c)
{
  faces.push_back(a);
  faces.push_back(b);
  faces.push_back(c);
  facecount += 1;
}

MaterialTexture::MaterialTexture(std::string p, EnumValue t)
    : path(std::move(p))
    , type(t)
{
}

Material::Material()
    : ambient(Rgb::From(Color::White))
    , diffuse(Rgb::From(Color::White))
    , specular(Rgb::From(Color::White))
    , shininess(42.0f)
    , alpha(1.0f)
    , wraps(WrapMode::REPEAT)
    , wrapt(WrapMode::REPEAT)
{
}

void
Material::SetTexture(const std::string& texture_name,
                     const std::string& texture_path)
{
  DEFINE_ENUM_VALUE(TextureType, texture_type, texture_name);
  textures.emplace_back(texture_path, texture_type);
}

namespace  // local
{
  DEFINE_ENUM_VALUE(TextureType, DiffuseType, "Diffuse");
}

namespace
{
  const unsigned int kAssimpFlags =
      aiProcess_CalcTangentSpace | aiProcess_Triangulate |
      aiProcess_SortByPType | aiProcess_FlipUVs | aiProcess_GenUVCoords |
      aiProcess_TransformUVCoords | aiProcess_OptimizeMeshes |
      aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices |
      aiProcess_ImproveCacheLocality | aiProcess_FindDegenerates |
      aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure |
      aiProcess_GenSmoothNormals | aiProcess_FindInvalidData;

  WrapMode
  GetTextureWrappingMode(const int mode)
  {
    switch(mode)
    {
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

  Rgb
  C(const aiColor3D c)
  {
    return Rgb{c.r, c.g, c.b};
  }

  void
  AddMaterials(Mesh* ret, const aiScene* scene)
  {
    for(unsigned int material_id = 0; material_id < scene->mNumMaterials;
        ++material_id)
    {
      const aiMaterial* mat = scene->mMaterials[material_id];

      Material material;

      if(mat->GetTextureCount(aiTextureType_DIFFUSE) <= 0)
      {
        // do nothing
      }
      else
      {
        aiString texture;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
        material.textures.emplace_back(texture.C_Str(), DiffuseType);
      }

      aiString aiName;
      mat->Get(AI_MATKEY_NAME, aiName);
      material.name = aiName.C_Str();

      const bool got_shininess =
          aiReturn_SUCCESS == mat->Get(AI_MATKEY_SHININESS, material.shininess);
      const bool got_alpha =
          aiReturn_SUCCESS == mat->Get(AI_MATKEY_OPACITY, material.alpha);

      if(!got_shininess)
      {
        material.shininess = 0.0f;
      }

      if(!got_alpha)
      {
        material.alpha = 1.0f;
      }

      aiColor3D aiAmbient;
      aiColor3D aiDiffuse;
      aiColor3D aiSpecular;
      mat->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);
      mat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
      mat->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);
      material.ambient  = C(aiAmbient);
      material.diffuse  = C(aiDiffuse);
      material.specular = C(aiSpecular);


      int u = 0;
      int v = 0;
      mat->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, 0), u);
      mat->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, 0), v);
      material.wraps = GetTextureWrappingMode(u);
      material.wrapt = GetTextureWrappingMode(v);

      // todo: improve texture detection?
      material.shader = "";
      ret->materials.push_back(material);
    }
  }

  void
  AddFaces(MeshPart* part, const aiMesh* mesh)
  {
    for(unsigned int face_id = 0; face_id < mesh->mNumFaces; ++face_id)
    {
      const aiFace& face = mesh->mFaces[face_id];
      part->AddFace(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }
  }

  void
  AddPoints(MeshPart* part, const aiMesh* mesh)
  {
    for(unsigned int index = 0; index < mesh->mNumVertices; ++index)
    {
      const aiVector3D& vertex = mesh->mVertices[index];
      const aiVector3D& normal = mesh->mNormals[index];
      float             u      = 0;
      float             v      = 0;
      if(mesh->HasTextureCoords(0))
      {
        const aiVector3D uv = mesh->mTextureCoords[0][index];
        u                   = uv.x;
        v                   = uv.y;
      }
      part->AddPoint(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z,
                     u, v);
    }
  }

  MeshPart
  ConvertMesh(const aiMesh* mesh)
  {
    MeshPart part;

    part.material = mesh->mMaterialIndex;
    AddPoints(&part, mesh);
    AddFaces(&part, mesh);

    return part;
  }

  Mesh
  ConvertScene(const aiScene* scene)
  {
    Mesh ret;

    /** @todo add parsing of nodes to the mesh so we could
    dynamically animate some rotors, wings etc. for example
     */

    if(scene->HasMeshes())
    {
      AddMaterials(&ret, scene);

      for(unsigned int meshid = 0; meshid < scene->mNumMeshes; ++meshid)
      {
        const aiMesh*  mesh = scene->mMeshes[meshid];
        const MeshPart part = ConvertMesh(mesh);
        ret.parts.push_back(part);
      }
    }

    return ret;
  }

  // http://assimp.sourceforge.net/howtoBasicShapes.html
  Mesh
  LoadFromString(const std::string& nff, const std::string& format)
  {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(
        nff.c_str(), nff.length() + 1, kAssimpFlags, format.c_str());
    if(scene == nullptr)
    {
      throw std::string{importer.GetErrorString()};
    }
    return ConvertScene(scene);
  }

  const char* const kFormatNff = "nff";
  const char*       kFormatObj = "obj";

}  // namespace

namespace meshes
{
  MeshLoadResult
  LoadMesh(const std::string& path)
  {
    Assimp::Importer importer;
    MeshLoadResult   res;

    const aiScene* scene = importer.ReadFile(path, kAssimpFlags);
    if(scene == nullptr)
    {
      res.error = importer.GetErrorString();
    }
    else
    {
      res.mesh = ConvertScene(scene);
    }
    return res;
  }

  Mesh
  CreateCube(float size)
  {
    return CreateBox(size, size, size);
  }

  Mesh
  CreateSphere(float size, const std::string& texture)
  {
    std::ostringstream ss;
    ss << "shader " << texture << std::endl << "s 0 0 0 " << size;
    return LoadFromString(ss.str(), kFormatNff);
  }

  Mesh
  CreateBox(float width, float height, float depth)
  {
    const float        x = width / 2;
    const float        y = height / 2;
    const float        z = depth / 2;
    std::ostringstream ss;
    ss << "v " << -x << " "
       << " " << -y << " " << -z << std::endl
       << "v " << -x << " "
       << " " << -y << " " << z << std::endl
       << "v " << -x << " "
       << " " << y << " " << -z << std::endl
       << "v " << -x << " "
       << " " << y << " " << z << std::endl
       << "v " << x << " "
       << " " << -y << " " << -z << std::endl
       << "v " << x << " "
       << " " << -y << " " << z << std::endl
       << "v " << x << " "
       << " " << y << " " << -z << std::endl
       << "v " << x << " "
       << " " << y << " " << z << std::endl
       << "" << std::endl
       << "vt 0 0" << std::endl
       << "vt 0 1" << std::endl
       << "vt 1 1" << std::endl
       << "vt 1 0" << std::endl
       << "" << std::endl
       << "f 3/1 7/2 5/3 1/4" << std::endl
       << "f 6/1 8/2 4/3 2/4" << std::endl
       << "f 2/1 4/2 3/3 1/4" << std::endl
       << "f 7/1 8/2 6/3 5/4" << std::endl
       << "f 4/1 8/2 7/3 3/4" << std::endl
       << "f 5/1 6/2 2/3 1/4" << std::endl;

    return LoadFromString(ss.str(), kFormatObj);
  }
}
