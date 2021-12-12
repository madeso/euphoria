#pragma once

#include <memory>
#include <map>

#include "core/mesh.h"
#include "core/mat4.h"
#include "render/buffer.h"


namespace euphoria::core::vfs
{
    struct DirPath;
}


namespace euphoria::render
{
    struct texture2d;
    struct material_shader;
    struct light;
    struct material_override;
    struct material_shader_cache;
    struct texture_cache;


    // one part of the mesh, single material
    struct compiled_mesh_part
    {
        // todo(Gustav): move vertex buffer and point layout to the compile mesh instead
        vertex_buffer data;

        // todo(Gustav): rename config to something better... like layout?
        point_layout config;
        index_buffer tris;
        int tri_count;
        unsigned int material;
    };


    struct compiled_mesh_material
    {
        compiled_mesh_material();


        void
        set_texture
        (
            const core::EnumValue& name,
            std::shared_ptr<texture2d> texture
        );


        void
        apply
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::Vec3f& camera,
            const light& light
        ) const;


        /** Gets the default materials from the shader if they are null/not set.
         */
        void
        load_default_materials_from_shader(texture_cache* cache);


        /** Asks the shader if all the textures are set,
         * and if more than necessary are set.
         */
        [[nodiscard]] bool
        validate() const;


        core::Rgb ambient;
        core::Rgb diffuse;
        core::Rgb specular;
        float shininess;
        std::shared_ptr<material_shader> shader;
        std::map<core::EnumValue, std::shared_ptr<texture2d>> textures;
    };


    /** A collection of parts making up a mesh.
     */
    struct compiled_mesh
    {
        std::vector<std::shared_ptr<compiled_mesh_part>> parts;
        std::vector<compiled_mesh_material> materials;


        void
        render
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::Vec3f& camera,
            const light& light,
            const std::shared_ptr<material_override>& overridden_materials
        );
    };


    std::shared_ptr<compiled_mesh>
    compile_mesh
    (
            const core::Mesh& mesh,
            material_shader_cache* shader_cache,
            texture_cache* texture_cache,
            const core::vfs::DirPath& texture_folder,
            const std::string& debug_name
    );
}
