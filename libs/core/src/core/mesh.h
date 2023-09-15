#pragma once




#include <optional>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/enum.h"
#include "core/aabb.h"
#include "core/vfs_path.h"

namespace eu::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }


    enum class WrapMode
    {
        repeat,
        clamp_to_edge,
        mirror_repeat
    };


    struct MeshPoint
    {
        vec3f vertex;
        vec3f normal;
        vec2f uv;

        MeshPoint
        (
            const vec3f& a_vertex,
            const vec3f& a_normal,
            const vec2f& a_uv
        );
    };


    struct MeshFace
    {
        int a;
        int b;
        int c;

        MeshFace(int a_a, int a_b, int a_c);
    };


    struct MeshPart
    {
        unsigned int material;
        std::vector<MeshPoint> points;
        std::vector<MeshFace> faces;

        MeshPart();

        [[nodiscard]] Aabb calc_aabb() const;
    };


    struct MaterialTexture
    {
        vfs::FilePath path;
        EnumValue type;

        MaterialTexture(const vfs::FilePath& p, EnumValue t);
    };


    struct Material
    {
        std::string name;
        std::optional<core::vfs::FilePath> shader;
        Rgb ambient;
        Rgb diffuse;
        Rgb specular;
        float shininess;
        float alpha;
        std::vector<MaterialTexture> textures;
        WrapMode wrap_s;
        WrapMode wrap_t;

        Material();

        void set_texture
        (
            const std::string& texture_name,
            const vfs::FilePath& texture_path
        );
    };


    struct Mesh
    {
        std::vector<Material> materials;
        std::vector<MeshPart> parts;

        [[nodiscard]] Aabb calc_aabb() const;
    };


    struct LoadedMeshOrError
    {
        Mesh loaded_mesh;
        std::string error;
    };


    namespace meshes
    {
        LoadedMeshOrError load_mesh(vfs::FileSystem* fs, const vfs::FilePath& path);

        Mesh create_cube(float size);
        Mesh create_sphere(float size, const std::string& texture);
        Mesh create_box(float width, float height, float depth);
    }

}
