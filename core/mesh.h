#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <vector>
#include <string>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/enum.h"
#include "core/aabb.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }

    enum class WrapMode
    {
        REPEAT,
        CLAMP_TO_EDGE,
        MIRROR_REPEAT
    };

    struct MeshPoint
    {
        MeshPoint(
                const vec3f& a_vertex,
                const vec3f& a_normal,
                const vec2f& a_uv);
        vec3f vertex;
        vec3f normal;
        vec2f uv;
    };

    struct MeshFace
    {
        MeshFace(int a_a, int a_b, int a_c);
        int a;
        int b;
        int c;
    };

    struct MeshPart
    {
        MeshPart();

        unsigned int           material;
        std::vector<MeshPoint> points;
        std::vector<MeshFace>  faces;

        Aabb
        CalculateAabb() const;
    };

    struct MaterialTexture
    {
        MaterialTexture(std::string p, EnumValue t);
        std::string path;
        EnumValue   type;
    };

    struct Material
    {
        Material();

        void
        SetTexture(
                const std::string& texture_name,
                const std::string& texture_path);

        std::string name;

        std::string shader;

        // tints
        Rgb ambient;
        Rgb diffuse;
        Rgb specular;

        float shininess;

        float                        alpha;
        std::vector<MaterialTexture> textures;
        WrapMode                     wraps;
        WrapMode                     wrapt;
    };

    struct Mesh
    {
        std::vector<Material> materials;
        std::vector<MeshPart> parts;

        Aabb
        CalculateAabb() const;
    };

    struct MeshLoadResult
    {
        Mesh        mesh;
        std::string error;
    };

    namespace meshes
    {
        MeshLoadResult
        LoadMesh(vfs::FileSystem* fs, const vfs::FilePath& path);

        Mesh
        CreateCube(float size);

        Mesh
        CreateSphere(float size, const std::string& texture);
        
        Mesh
        CreateBox(float width, float height, float depth);
    }  // namespace meshes

}  // namespace euphoria::core

#endif  // CORE_MESH_H
