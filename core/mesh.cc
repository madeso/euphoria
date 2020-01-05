#include "core/mesh.h"

#include "core/cint.h"
#include "core/str.h"
#include "core/texturetypes.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/stringmerger.h"
#include "core/vfs.h"

#include "assimp/Importer.hpp"
#include "assimp/IOSystem.hpp"
#include "assimp/IOStream.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <sstream>
#include <utility>

#include "gaf_mesh.h"


namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.mesh")


    MeshPoint::MeshPoint
    (
        const vec3f& a_vertex,
        const vec3f& a_normal,
        const vec2f& a_uv
    )
        : vertex(a_vertex)
        , normal(a_normal)
        , uv(a_uv)
    {}


    MeshFace::MeshFace(int a_a, int a_b, int a_c)
        : a(a_a)
        , b(a_b)
        , c(a_c)
    {}


    template <typename K, typename V>
    std::vector<K>
    KeysOf(const std::map<K, V>& m)
    {
        std::vector<K> r;
        for(const auto& p: m)
        {
            r.emplace_back(p.first);
        }
        return r;
    }

    MeshPart::MeshPart() : material(0) {}

    Aabb
    MeshPart::CalculateAabb() const
    {
        Aabb aabb = Aabb::Empty();

        for(const auto& p: points)
        {
            aabb.Extend(p.vertex);
        }

        return aabb;
    }


    MaterialTexture::MaterialTexture(const vfs::FilePath& p, EnumValue t)
        : path(p)
        , type(t)
    {}


    Material::Material()
        : name("unknown_material")
        , shader(std::nullopt)
        , ambient(Color::White)
        , diffuse(Color::White)
        , specular(Color::White)
        , shininess(42.0f)
        , alpha(1.0f)
        , wraps(WrapMode::REPEAT)
        , wrapt(WrapMode::REPEAT)
    {}


    void
    Material::SetTexture
    (
        const std::string& texture_name,
        const vfs::FilePath& texture_path
    )
    {
        DEFINE_ENUM_VALUE(TextureType, texture_type, texture_name);
        textures.emplace_back(texture_path, texture_type);
    }


    Aabb
    Mesh::CalculateAabb() const
    {
        Aabb aabb = Aabb::Empty();

        for(const auto& part: parts)
        {
            aabb.Extend(part.CalculateAabb());
        }

        return aabb;
    }


    namespace  // local
    {
        DEFINE_ENUM_VALUE(TextureType, DiffuseType, "Diffuse");  // NOLINT
    }  // namespace


    namespace
    {
        const unsigned int AssimpFlags
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

        WrapMode
        GetTextureWrappingMode(const int mode)
        {
            switch(mode)
            {
            case aiTextureMapMode_Wrap: return WrapMode::REPEAT;
            case aiTextureMapMode_Clamp: return WrapMode::CLAMP_TO_EDGE;
            case aiTextureMapMode_Decal:
                throw "Unsupported texture wrapping mode: decal";
            case aiTextureMapMode_Mirror: return WrapMode::MIRROR_REPEAT;
            default: throw "Unhandled texture wrapping mode";
            }
        }


        Rgb
        C(const aiColor3D c)
        {
            return Rgb {c.r, c.g, c.b};
        }


        void
        AddMaterials(Mesh* ret, const aiScene* scene)
        {
            for(unsigned int material_id = 0;
                material_id < scene->mNumMaterials;
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
                    auto path = core::vfs::FilePath::FromDirtySource(texture.C_Str());
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
                        path.value_or(core::vfs::FilePath{"~/image-plain/blue"}),
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
                material.ambient  = C(ai_ambient);
                material.diffuse  = C(ai_diffuse);
                material.specular = C(ai_specular);


                int u = 0;
                int v = 0;
                mat->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, 0), u);
                mat->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, 0), v);
                material.wraps = GetTextureWrappingMode(u);
                material.wrapt = GetTextureWrappingMode(v);

                // todo: improve texture detection?
                material.shader = std::nullopt;
                ret->materials.push_back(material);
            }
        }


        void
        AddFaces(MeshPart* part, const aiMesh* mesh)
        {
            for(unsigned int face_id = 0; face_id < mesh->mNumFaces; ++face_id)
            {
                const aiFace& face = mesh->mFaces[face_id];
                if(face.mNumIndices<3) continue;
                ASSERTX(face.mNumIndices == 3, face.mNumIndices); 
                const auto faceIndex0 = face.mIndices[0];
                const auto faceIndex1 = face.mIndices[1];
                const auto faceIndex2 = face.mIndices[2];
                part->faces.push_back(MeshFace
                {
                    Cunsigned_int_to_int(faceIndex0),
                    Cunsigned_int_to_int(faceIndex1),
                    Cunsigned_int_to_int(faceIndex2)
                });
            }
        }


        void
        AddPoints(MeshPart* part, const aiMesh* mesh)
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
                    MeshPoint
                    {
                        vec3f {vertex.x, vertex.y, vertex.z},
                        vec3f {normal.x, normal.y, normal.z},
                        vec2f {u, v}}
                );
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
        ConvertScene(const aiScene* scene, const std::string& file_name)
        {
            Mesh ret;

            /** @todo add parsing of nodes to the mesh so we could
                dynamically animate some rotors, wings etc. for example
                */

            if(scene->HasMeshes())
            {
                AddMaterials(&ret, scene);

                for(unsigned int meshid = 0; meshid < scene->mNumMeshes;
                    ++meshid)
                {
                    const aiMesh*  mesh = scene->mMeshes[meshid];
                    const MeshPart part = ConvertMesh(mesh);
                    if(part.faces.empty())
                    {
                        const auto& name = mesh->mName;
                        const char* const the_name = name.C_Str() ? name.C_Str() : "<no_name>";
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
        Mesh
        LoadFromString(const std::string& nff, const std::string& format)
        {
            Assimp::Importer importer;

            const aiScene* scene = importer.ReadFileFromMemory(
                    nff.c_str(), nff.length() + 1, AssimpFlags, format.c_str());
            if(scene == nullptr)
            {
                throw std::string {importer.GetErrorString()};
            }
            return ConvertScene(scene, "<nff_source>");
        }


        const char* const FileFormatNff = "nff";
        const char* const FileFormatObj = "obj";


        void
        DecorateMeshMaterials
        (
            Mesh* mesh,
            const vfs::FilePath& json_path,
            const mesh::Mesh& json
        )
        {
            std::map<std::string, Material*> mesh_materials;
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
                        StringMerger::EnglishOr().Generate(KeysOf(mesh_materials))
                    );
                    continue;
                }

                auto* other = found->second;
                for(const auto& src_texture: material.textures)
                {
                    auto path = core::vfs::FilePath::FromScript
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
                    other->SetTexture
                    (
                        src_texture.type,
                        path.value()
                    );
                }
            }
        }


        void
        DecorateMeshMaterialsIgnoreAmbient(Mesh* mesh)
        {
            for(auto& material: mesh->materials)
            {
                material.ambient = material.diffuse;
            }
        }


        void
        FixExtension(vfs::FilePath* path, const mesh::Folder& folder)
        {
            const auto ext = path->GetExtension();
            for(auto c: folder.change_extensions)
            {
                if(ext == c.old_ext)
                {
                    const auto new_path = path->SetExtensionCopy(c.new_ext);
                    LOG_INFO("Changing extension from {0} to {1}", *path, new_path);
                    *path = new_path;
                    return;
                }
            }
        }


        void
        DecorateMesh
        (
            vfs::FileSystem* fs,
            Mesh* mesh,
            const vfs::FilePath& json_path
        )
        {
            mesh::Mesh json;
            const auto error = LoadProtoJson(fs, &json, json_path);
            if(!error.empty())
            {
                // LOG_WARN("Mesh " << json_path << " failed to load: " << error);
            }

            if(json.diffuse_and_ambient_are_same)
            {
                DecorateMeshMaterialsIgnoreAmbient(mesh);
            }

            if(!json.materials.empty())
            {
                DecorateMeshMaterials(mesh, json_path, json);
            }

            const auto json_dir = json_path.GetDirectory();
            const auto folder_path = json_dir.GetFile("folder.json");
            mesh::Folder folder;
            const auto folder_error = LoadProtoJson(fs, &folder, folder_path);
            if(!folder_error.empty())
            {
                return;
            }
            if(folder.texture_override.empty())
            {
                return;
            }
            auto dir = vfs::DirPath{folder.texture_override};
            if(dir.IsRelative()) { dir = vfs::Join(json_dir, dir); }

            for(auto& m: mesh->materials)
            {
                for(auto& t: m.textures)
                {
                    const auto new_file = dir.GetFile(t.path.GetFileWithExtension());
                    // LOG_INFO("Replacing {0} with {1}", t.path, new_file);
                    t.path = new_file;
                    FixExtension(&t.path, folder);
                }
            }

        }
    }  // namespace


    namespace meshes
    {
        struct FileForAssimp : public Assimp::IOStream
        {
            size_t index = 0;
            std::shared_ptr<MemoryChunk> content;

            size_t Read(void* target_buffer, size_t size, size_t count) override
            {
                char* target = static_cast<char*>(target_buffer);
                size_t objects_read = 0;
                for(size_t i =0; i<count; i+=1)
                {
                    if(Csizet_to_int(index + size) > content->GetSize())
                    {
                        return objects_read;
                    }
                    memcpy(target, content->GetData() + index, size);
                    index += size;
                    target += size;
                    objects_read += 1;
                }
                return objects_read;
            }

            virtual size_t Write(const void* , size_t , size_t ) override
            {
                return 0;
            }

            virtual aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
            {
                switch(pOrigin)
                {
                    case aiOrigin_SET: index = pOffset; return aiReturn_SUCCESS;
                    case aiOrigin_CUR: index += pOffset; return aiReturn_SUCCESS;
                    case aiOrigin_END: DIE("end seek with unsinged int?"); return aiReturn_FAILURE;
                    default: DIE("unknown seek"); return aiReturn_FAILURE;
                }
            }

            virtual size_t Tell() const override
            {
                return index;
            }

            virtual size_t FileSize() const override
            {
                return content->GetSize();
            }

            virtual void Flush() override
            {
            }
        };

        
        struct FilesystemForAssimp : public Assimp::IOSystem
        {
            vfs::FileSystem* file_system;

            FilesystemForAssimp(vfs::FileSystem* fs) : file_system(fs) {}

            bool Exists( const char* pFile) const override
            {
                auto content = file_system->ReadFile(vfs::FilePath{pFile});
                return content != nullptr;
            }

            char getOsSeparator() const override
            {
                return '/';
            }

            Assimp::IOStream* Open(const char* pFile, const char* pMode) override
            {
                std::string mode = pMode;
                ASSERT(mode.find('w') == std::string::npos);
                ASSERT(mode.find('r') != std::string::npos);
                auto content = file_system->ReadFile(vfs::FilePath{pFile});
                if(content == nullptr)
                {
                    return nullptr;
                }
                auto* data = new FileForAssimp();
                data->content = content;
                return data;
            }
            
            void Close(Assimp::IOStream* pFile) override
            {
                delete pFile;
            }

            bool DeleteFile( const std::string& ) override
            {
                return false;
            }
        };


        MeshLoadResult
        LoadMesh(vfs::FileSystem* fs, const vfs::FilePath& path)
        {
            Assimp::Importer importer;
            importer.SetIOHandler(new FilesystemForAssimp{fs});

            MeshLoadResult   res;

            const aiScene* scene = importer.ReadFile(path.path, AssimpFlags);
            if(scene == nullptr)
            {
                res.error = importer.GetErrorString();
            }
            else
            {
                res.mesh = ConvertScene(scene, path.path);
                DecorateMesh
                (
                    fs,
                    &res.mesh,
                    path.SetExtensionCopy(path.GetExtension()+".json")
                );

                if(res.mesh.parts.empty())
                {
                    res.error = "No parts(faces) in mesh";
                }
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
            return LoadFromString(ss.str(), FileFormatNff);
        }


        Mesh
        CreateBox(float width, float height, float depth)
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

            auto box = LoadFromString(ss.str(), FileFormatObj);
            return box;
        }
    }  // namespace meshes

}  // namespace euphoria::core
