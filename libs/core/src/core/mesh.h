#pragma once




#include <optional>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/enum.h"
#include "core/aabb.h"
#include "core/vfs_path.h"

namespace euphoria::core
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
        MeshPoint
        (
            const Vec3f& a_vertex,
            const Vec3f& a_normal,
            const Vec2f& a_uv
        );
        Vec3f vertex;
        Vec3f normal;
        Vec2f uv;
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

        unsigned int material;
        std::vector<MeshPoint> points;
        std::vector<MeshFace> faces;

        [[nodiscard]] Aabb
        calculate_aabb() const;
    };


    struct MaterialTexture
    {
        MaterialTexture(const vfs::FilePath& p, EnumValue t);
        vfs::FilePath path;
        EnumValue type;
    };


    struct Material
    {
        Material();

        void
        set_texture
        (
            const std::string& texture_name,
            const vfs::FilePath& texture_path
        );

        std::string name;

        std::optional<core::vfs::FilePath> shader;

        // tints
        Rgb ambient;
        Rgb diffuse;
        Rgb specular;

        float shininess;

        float alpha;
        std::vector<MaterialTexture> textures;
        WrapMode wrap_s;
        WrapMode wrap_t;
    };


    struct Mesh
    {
        std::vector<Material> materials;
        std::vector<MeshPart> parts;

        [[nodiscard]] Aabb
        calculate_aabb() const;
    };


    struct LoadedMeshOrError
    {
        Mesh loaded_mesh;
        std::string error;
    };


    namespace meshes
    {
        LoadedMeshOrError
        load_mesh(vfs::FileSystem* fs, const vfs::FilePath& path);

        Mesh
        create_cube(float size);

        Mesh
        create_sphere(float size, const std::string& texture);

        Mesh
        create_box(float width, float height, float depth);
    }

}
