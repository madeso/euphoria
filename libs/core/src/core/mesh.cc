#include "core/mesh.h"

#include <sstream>
#include <utility>

#include "assimp/Importer.hpp"
#include "assimp/IOSystem.hpp"
#include "assimp/IOStream.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "core/cint.h"
#include "core/str.h"
#include "core/texturetypes.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/stringmerger.h"
#include "core/vfs.h"

#include "gaf_mesh.h"


#define OPTIONAL_OVERRIDE

namespace euphoria::core
{
    mesh_point::mesh_point
    (
        const vec3f& a_vertex,
        const vec3f& a_normal,
        const vec2f& a_uv
    )
        : vertex(a_vertex)
        , normal(a_normal)
        , uv(a_uv)
    {}


    mesh_face::mesh_face(int a_a, int a_b, int a_c)
        : a(a_a)
        , b(a_b)
        , c(a_c)
    {
    }

    mesh_part::mesh_part() : material(0) {}

    aabb
    mesh_part::calculate_aabb() const
    {
        aabb aabb = core::aabb::create_empty();

        for(const auto& p: points)
        {
            aabb.extend(p.vertex);
        }

        return aabb;
    }


    material_texture::material_texture(const vfs::file_path& p, enum_value t)
        : path(p)
        , type(t)
    {}


    material::material()
        : name("unknown_material")
        , shader(std::nullopt)
        , ambient(color::white)
        , diffuse(color::white)
        , specular(color::white)
        , shininess(42.0f)
        , alpha(1.0f)
        , wrap_s(wrap_mode::repeat)
        , wrap_t(wrap_mode::repeat)
    {}


    void
    material::set_texture
    (
        const std::string& texture_name,
        const vfs::file_path& texture_path
    )
    {
        DEFINE_ENUM_VALUE(texture_type, texture_type, texture_name);
        textures.emplace_back(texture_path, texture_type);
    }


    aabb
    mesh::calculate_aabb() const
    {
        aabb aabb = core::aabb::create_empty();

        for(const auto& part: parts)
        {
            aabb.extend(part.calculate_aabb());
        }

        return aabb;
    }


    namespace // local
    {
        DEFINE_ENUM_VALUE(texture_type, DiffuseType, "Diffuse"); // NOLINT
    } // namespace


    namespace
    {
        constexpr unsigned int assimp_flags
                = aiProcess_CalcTangentSpace
                | aiProcess_Triangulate
                | aiProcess_SortByPType
                | aiProcess_FlipUVs
                | aiProcess_GenUVCoords
                | aiProcess_TransformUVCoords
                | aiProcess_OptimizeMeshes
                | aiProcess_RemoveRedundantMaterials
                | aiProcess_PreTransformVertices
                | aiProcess_ImproveCacheLocality
                | aiProcess_FindDegenerates
                | aiProcess_JoinIdenticalVertices
                | aiProcess_ValidateDataStructure
                | aiProcess_GenSmoothNormals
                | aiProcess_FindInvalidData;

        wrap_mode
        get_texture_wrapping_mode(const int mode)
        {
            switch(mode)
            {
            case aiTextureMapMode_Wrap: return wrap_mode::repeat;
            case aiTextureMapMode_Clamp: return wrap_mode::clamp_to_edge;
            case aiTextureMapMode_Decal:
                throw "Unsupported texture wrapping mode: decal";
            case aiTextureMapMode_Mirror: return wrap_mode::mirror_repeat;
            default: throw "Unhandled texture wrapping mode";
            }
        }


        rgb
        C(const aiColor3D c)
        {
            return rgb {c.r, c.g, c.b};
        }


        void
        add_materials(mesh* ret, const aiScene* scene)
        {
            for(unsigned int material_id = 0;
                material_id < scene->mNumMaterials;
                ++material_id)
            {
                const aiMaterial* mat = scene->mMaterials[material_id];

                material material;

                if(mat->GetTextureCount(aiTextureType_DIFFUSE) <= 0)
                {
                    // do nothing
                }
                else
                {
                    aiString texture;
                    mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
                    auto path = core::vfs::file_path::from_dirty_source(texture.C_Str());
                    if(path.has_value() == false)
                    {
                        LOG_WARN
                        (
                            "invalid diffuse texture for material {0}: {0}",
                            material_id,
                            texture.C_Str()
                        );
                    }
                    material.textures.emplace_back
                    (
                        path.value_or(core::vfs::file_path{"~/image-plain/blue"}),
                        DiffuseType
                    );
                }

                aiString ai_name;
                mat->Get(AI_MATKEY_NAME, ai_name);
                material.name = ai_name.C_Str();

                const bool got_shininess
                        = aiReturn_SUCCESS
                          == mat->Get(AI_MATKEY_SHININESS, material.shininess);
                const bool got_alpha
                        = aiReturn_SUCCESS
                          == mat->Get(AI_MATKEY_OPACITY, material.alpha);

                if(!got_shininess)
                {
                    material.shininess = 0.0f;
                }

                if(!got_alpha)
                {
                    material.alpha = 1.0f;
                }

                aiColor3D ai_ambient;
                aiColor3D ai_diffuse;
                aiColor3D ai_specular;
                mat->Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient);
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_diffuse);
                mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_specular);
                material.ambient = C(ai_ambient);
                material.diffuse = C(ai_diffuse);
                material.specular = C(ai_specular);


                int u = 0;
                int v = 0;
                mat->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, 0), u);
                mat->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, 0), v);
                material.wrap_s = get_texture_wrapping_mode(u);
                material.wrap_t = get_texture_wrapping_mode(v);

                // todo(Gustav): improve texture detection?
                material.shader = std::nullopt;
                ret->materials.push_back(material);
            }
        }


        void
        add_faces(mesh_part* part, const aiMesh* mesh)
        {
            for(unsigned int face_id = 0; face_id < mesh->mNumFaces; ++face_id)
            {
                const aiFace& face = mesh->mFaces[face_id];
                if(face.mNumIndices<3)
                {
                    continue;
                }
                ASSERTX(face.mNumIndices == 3, face.mNumIndices);
                part->faces.emplace_back
                (
                    Cunsigned_int_to_int(face.mIndices[0]),
                    Cunsigned_int_to_int(face.mIndices[1]),
                    Cunsigned_int_to_int(face.mIndices[2])
                );
            }
        }


        void
        add_points(mesh_part* part, const aiMesh* mesh)
        {
            for(unsigned int index = 0; index < mesh->mNumVertices; ++index)
            {
                const aiVector3D& vertex = mesh->mVertices[index];
                const aiVector3D& normal = mesh->mNormals[index];
                float u = 0;
                float v = 0;
                if(mesh->HasTextureCoords(0))
                {
                    const aiVector3D uv = mesh->mTextureCoords[0][index];
                    u = uv.x;
                    v = uv.y;
                }
                part->points.push_back
                (
                    mesh_point
                    {
                        vec3f {vertex.x, vertex.y, vertex.z},
                        vec3f {normal.x, normal.y, normal.z},
                        vec2f {u, v}}
                );
            }
        }


        mesh_part
        convert_mesh(const aiMesh* mesh)
        {
            mesh_part part;

            part.material = mesh->mMaterialIndex;
            add_points(&part, mesh);
            add_faces(&part, mesh);

            return part;
        }


        mesh
        convert_scene(const aiScene* scene, const std::string& file_name)
        {
            mesh ret;

            /** @todo add parsing of nodes to the mesh so we could
                dynamically animate some rotors, wings etc. for example
                */

            if(scene->HasMeshes())
            {
                add_materials(&ret, scene);

                for(unsigned int meshid = 0; meshid < scene->mNumMeshes;
                    ++meshid)
                {
                    const aiMesh* mesh = scene->mMeshes[meshid];
                    const mesh_part part = convert_mesh(mesh);
                    if(part.faces.empty())
                    {
                        const auto& name = mesh->mName;
                        const char* const the_name = name.C_Str() != nullptr
                            ? name.C_Str()
                            : "<no_name>"
                            ;
                        LOG_WARN("No faces in part {0} for {1}", the_name, file_name);
                    }
                    else
                    {
                        ret.parts.push_back(part);
                    }
                }
            }

            return ret;
        }


        // http://assimp.sourceforge.net/howtoBasicShapes.html
        mesh
        load_from_string(const std::string& nff, const std::string& format)
        {
            Assimp::Importer importer;

            const aiScene* scene = importer.ReadFileFromMemory(
                    nff.c_str(), nff.length() + 1, assimp_flags, format.c_str());
            if(scene == nullptr)
            {
                throw std::string {importer.GetErrorString()};
            }
            return convert_scene(scene, "<nff_source>");
        }


        const char* const FileFormatNff = "nff";
        const char* const FileFormatObj = "obj";


        void
        decorate_mesh_materials
        (
            mesh* mesh,
            const vfs::file_path& json_path,
            const ::mesh::Mesh& json
        )
        {
            std::map<std::string, material*> mesh_materials;
            for(auto& material: mesh->materials)
            {
                mesh_materials[material.name] = &material;
            }

            for(const auto& material: json.materials)
            {
                auto found = mesh_materials.find(material.name);
                if(found == mesh_materials.end())
                {
                    LOG_ERROR
                    (
                        "Unable to find {0} in mesh {1} valid names are: {2}",
                        material.name,
                        json_path,
                        string_mergers::english_or.merge(get_keys(mesh_materials))
                    );
                    continue;
                }

                auto* other = found->second;
                for(const auto& src_texture: material.textures)
                {
                    auto path = core::vfs::file_path::from_script
                    (
                        src_texture.path
                    );
                    if(path.has_value() == false)
                    {
                        LOG_WARN
                        (
                            "Invalid path {0} in mesh {1}",
                            src_texture.path,
                            json_path
                        );
                        continue;
                    }
                    other->set_texture
                    (
                        src_texture.type,
                        path.value()
                    );
                }
            }
        }


        void
        decorate_mesh_materials_ignore_ambient(mesh* mesh)
        {
            for(auto& material: mesh->materials)
            {
                material.ambient = material.diffuse;
            }
        }


        void
        fix_extension(vfs::file_path* path, const ::mesh::Folder& folder)
        {
            const auto ext = path->get_extension();
            for(auto c: folder.change_extensions)
            {
                if(ext == c.old_ext)
                {
                    const auto new_path = path->set_extension_copy(c.new_ext);
                    *path = new_path;
                    return;
                }
            }
        }


        void
        fix_filename(vfs::file_path* path, const ::mesh::Folder& folder)
        {
            const auto [dir, file] = path->split_directories_and_file();
            for(auto c: folder.change_filenames)
            {
                if(file == c.old_file)
                {
                    const auto new_path = dir.get_file(c.new_file);
                    *path = new_path;
                    return;
                }
            }
        }


        void
        decorate_mesh
        (
            vfs::file_system* fs,
            mesh* mesh,
            const vfs::file_path& json_path
        )
        {
            ::mesh::Mesh json;
            const auto error = read_json_to_gaf_struct_or_get_error_message(fs, &json, json_path);
            if(!error.empty())
            {
                // LOG_WARN("Mesh " << json_path << " failed to load: " << error);
            }

            if(json.diffuse_and_ambient_are_same)
            {
                decorate_mesh_materials_ignore_ambient(mesh);
            }

            if(!json.materials.empty())
            {
                decorate_mesh_materials(mesh, json_path, json);
            }

            const auto json_dir = json_path.get_directory();
            const auto folder_path = json_dir.get_file("folder.json");
            ::mesh::Folder folder;
            const auto folder_error = read_json_to_gaf_struct_or_get_error_message(fs, &folder, folder_path);
            if(!folder_error.empty())
            {
                return;
            }
            if(folder.texture_override.empty())
            {
                return;
            }
            auto dir = vfs::dir_path{folder.texture_override};
            if(dir.is_relative()) { dir = vfs::join(json_dir, dir); }

            for(auto& p: mesh->parts)
            {
                for(auto& v: p.points)
                {
                    v.vertex = v.vertex * folder.scale;
                }
            }

            for(auto& m: mesh->materials)
            {
                for(auto& t: m.textures)
                {
                    const auto new_file = dir.get_file(t.path.get_file_with_extension());
                    // LOG_INFO("Replacing {0} with {1}", t.path, new_file);
                    t.path = new_file;
                    fix_extension(&t.path, folder);
                    fix_filename(&t.path, folder);
                }
            }

        }
    } // namespace


    namespace meshes
    {
        struct file_for_assimp : public Assimp::IOStream
        {
            size_t index = 0;
            std::shared_ptr<memory_chunk> content;

            size_t Read(void* target_buffer, size_t size, size_t count) override
            {
                char* target = static_cast<char*>(target_buffer);
                size_t objects_read = 0;
                for(size_t i =0; i<count; i+=1)
                {
                    if(Csizet_to_int(index + size) > content->get_size())
                    {
                        return objects_read;
                    }
                    memcpy(target, content->get_data() + index, size);
                    index += size;
                    target += size;
                    objects_read += 1;
                }
                return objects_read;
            }

            size_t Write(const void* , size_t , size_t ) override
            {
                return 0;
            }

            aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
            {
                switch(pOrigin)
                {
                    case aiOrigin_SET: index = pOffset; return aiReturn_SUCCESS;
                    case aiOrigin_CUR: index += pOffset; return aiReturn_SUCCESS;
                    case aiOrigin_END: DIE("end seek with unsinged int?"); return aiReturn_FAILURE;
                    default: DIE("unknown seek"); return aiReturn_FAILURE;
                }
            }

            [[nodiscard]] size_t Tell() const override
            {
                return index;
            }

            [[nodiscard]] size_t FileSize() const override
            {
                return content->get_size();
            }

            void Flush() override
            {
            }
        };


        struct filesystem_for_assimp : public Assimp::IOSystem
        {
            vfs::file_system* file_system;

            filesystem_for_assimp(vfs::file_system* fs) : file_system(fs) {}

            bool Exists(const char* pFile) const override
            {
                auto content = file_system->read_file(vfs::file_path{pFile});
                return content != nullptr;
            }

            [[nodiscard]] char getOsSeparator() const override
            {
                return '/';
            }

            Assimp::IOStream* Open(const char* pFile, const char* pMode) override
            {
                std::string mode = pMode;
                ASSERT(mode.find('w') == std::string::npos);
                ASSERT(mode.find('r') != std::string::npos);
                auto content = file_system->read_file(vfs::file_path{pFile});
                if(content == nullptr)
                {
                    return nullptr;
                }
                auto* data = new file_for_assimp(); // NOLINT
                data->content = content;
                return data;
            }

            void Close(Assimp::IOStream* pFile) override
            {
                delete pFile; // NOLINT
            }

            bool DeleteFile( const std::string& ) OPTIONAL_OVERRIDE
            {
                return false;
            }
        };


        loaded_mesh_or_error
        load_mesh(vfs::file_system* fs, const vfs::file_path& path)
        {
            Assimp::Importer importer;
            importer.SetIOHandler(new filesystem_for_assimp{fs}); // NOLINT

            loaded_mesh_or_error res;

            const aiScene* scene = importer.ReadFile(path.path, assimp_flags);
            if(scene == nullptr)
            {
                res.error = importer.GetErrorString();
            }
            else
            {
                res.loaded_mesh = convert_scene(scene, path.path);
                decorate_mesh
                (
                    fs,
                    &res.loaded_mesh,
                    path.set_extension_copy(path.get_extension()+".json")
                );

                if(res.loaded_mesh.parts.empty())
                {
                    res.error = "No parts(faces) in mesh";
                }
            }
            return res;
        }


        mesh
        create_cube(float size)
        {
            return create_box(size, size, size);
        }


        mesh
        create_sphere(float size, const std::string& texture)
        {
            std::ostringstream ss;
            ss << "shader " << texture << std::endl << "s 0 0 0 " << size;
            return load_from_string(ss.str(), FileFormatNff);
        }


        mesh
        create_box(float width, float height, float depth)
        {
            const float x = width / 2;
            const float y = height / 2;
            const float z = depth / 2;
            std::ostringstream ss;
            ss << "v " << -x << " " << " " << -y << " " << -z << std::endl
               << "v " << -x << " " << " " << -y << " " << z << std::endl
               << "v " << -x << " " << " " << y << " " << -z << std::endl
               << "v " << -x << " " << " " << y << " " << z << std::endl
               << "v " << x << " " << " " << -y << " " << -z << std::endl
               << "v " << x << " " << " " << -y << " " << z << std::endl
               << "v " << x << " " << " " << y << " " << -z << std::endl
               << "v " << x << " " << " " << y << " " << z << std::endl
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

            auto box = load_from_string(ss.str(), FileFormatObj);
            return box;
        }
    }
}
