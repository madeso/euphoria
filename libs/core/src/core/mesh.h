#pragma once


#include <vector>
#include <string>
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
        struct file_system;
        struct file_path;
    }


    enum class wrap_mode
    {
        repeat,
        clamp_to_edge,
        mirror_repeat
    };


    struct mesh_point
    {
        mesh_point
        (
            const vec3f& a_vertex,
            const vec3f& a_normal,
            const vec2f& a_uv
        );
        vec3f vertex;
        vec3f normal;
        vec2f uv;
    };


    struct mesh_face
    {
        mesh_face(int a_a, int a_b, int a_c);
        int a;
        int b;
        int c;
    };


    struct mesh_part
    {
        mesh_part();

        unsigned int material;
        std::vector<mesh_point> points;
        std::vector<mesh_face> faces;

        [[nodiscard]] aabb
        calculate_aabb() const;
    };


    struct material_texture
    {
        material_texture(const vfs::file_path& p, enum_value t);
        vfs::file_path path;
        enum_value type;
    };


    struct material
    {
        material();

        void
        set_texture
        (
            const std::string& texture_name,
            const vfs::file_path& texture_path
        );

        std::string name;

        std::optional<core::vfs::file_path> shader;

        // tints
        rgb ambient;
        rgb diffuse;
        rgb specular;

        float shininess;

        float alpha;
        std::vector<material_texture> textures;
        wrap_mode wrap_s;
        wrap_mode wrap_t;
    };


    struct mesh
    {
        std::vector<material> materials;
        std::vector<mesh_part> parts;

        [[nodiscard]] aabb
        calculate_aabb() const;
    };


    struct loaded_mesh_or_error
    {
        mesh loaded_mesh;
        std::string error;
    };


    namespace meshes
    {
        loaded_mesh_or_error
        load_mesh(vfs::file_system* fs, const vfs::file_path& path);

        mesh
        create_cube(float size);

        mesh
        create_sphere(float size, const std::string& texture);

        mesh
        create_box(float width, float height, float depth);
    } // namespace meshes

} // namespace euphoria::core

