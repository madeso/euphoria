#pragma once

#include <memory>
#include <map>

#include "core/mesh.h"
#include "base/mat4.h"
#include "render/buffer.h"


namespace eu::io
{
    struct DirPath;
}


namespace eu::render
{
    struct Texture2;
    struct MaterialShader;
    struct Light;
    struct MaterialOverride;
    struct MaterialShaderCache;
    struct TextureCache;


    // one part of the mesh, single material
    struct CompiledMeshPart
    {
        // todo(Gustav): move vertex buffer and point layout to the compile mesh instead
        VertexBuffer data;

        // todo(Gustav): rename config to something better... like layout?
        PointLayout config;
        IndexBuffer tris;
        int tri_count;
        unsigned int material;
    };


    struct CompiledMeshMaterial
    {
        Rgb ambient;
        Rgb diffuse;
        Rgb specular;
        float shininess;
        std::shared_ptr<MaterialShader> shader;
        std::map<core::EnumValue, std::shared_ptr<Texture2>> textures;


        CompiledMeshMaterial();

        void set_texture
        (
            const core::EnumValue& name,
            std::shared_ptr<Texture2> texture
        );

        /** Gets the default materials from the shader if they are null/not set.
         */
        void load_default_materials_from_shader(TextureCache* cache);

        void apply
        (
            const mat4f& model_matrix,
            const mat4f& projection_matrix,
            const mat4f& view_matrix,
            const vec3f& camera,
            const Light& light
        ) const;

        /** Asks the shader if all the textures are set,
         * and if more than necessary are set.
         */
        [[nodiscard]] bool validate() const;
    };


    /** A collection of parts making up a mesh.
     */
    struct CompiledMesh
    {
        std::vector<std::shared_ptr<CompiledMeshPart>> parts;
        std::vector<CompiledMeshMaterial> materials;


        void
        render
        (
            const mat4f& model_matrix,
            const mat4f& projection_matrix,
            const mat4f& view_matrix,
            const vec3f& camera,
            const Light& light,
            const std::shared_ptr<MaterialOverride>& overridden_materials
        );
    };


    std::shared_ptr<CompiledMesh>
    compile_mesh
    (
            const core::Mesh& mesh,
            MaterialShaderCache* shader_cache,
            TextureCache* texture_cache,
            const io::DirPath& texture_folder,
            const std::string& debug_name
    );
}
