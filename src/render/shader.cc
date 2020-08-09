#include "render/shader.h"

#include "core/assert.h"
#include "core/log.h"
#include "core/stringutils.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "render/gl.h"
#include "render/texture.h"

#include "core/vfs.h"

LOG_SPECIFY_DEFAULT_LOGGER("render.shader")

namespace euphoria::render
{
    ShaderId::ShaderId() : id_(glCreateProgram()) {}

    ShaderId::~ShaderId()
    {
        glDeleteProgram(id_);
    }

    GLuint
    ShaderId::GetId() const
    {
        return id_;
    }

    namespace
    {
        const Shader*&
        GetCurrentShader()
        {
            static const Shader* CurrentShader = nullptr;
            return CurrentShader;
        }
    }  // namespace

    bool
    ShaderId::IsCurrentlyBound() const
    {
        return this == GetCurrentShader();
    }

    void
    Use(const Shader* shader)
    {
        if(shader != nullptr)
        {
            glUseProgram(shader->GetId());
        }
        else
        {
            glUseProgram(0);
        }
        GetCurrentShader() = shader;
    }

    const Shader*
    Shader::CurrentlyBound()
    {
        return GetCurrentShader();
    }

    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        bool
        WasCompilationSuccessful(GLuint object)
        {
            int r = GL_TRUE;
            glGetShaderiv(object, GL_COMPILE_STATUS, &r);
            return r == GL_TRUE;
        }

        bool
        WasLinkingSuccessful(GLuint object)
        {
            int r = GL_TRUE;
            glGetProgramiv(object, GL_LINK_STATUS, &r);
            return r == GL_TRUE;
        }

        std::string
        GetShaderLog(GLuint shader)
        {
            int length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if(length <= 0)
            {
                return "";
            }
            const int         max_length = length + 1;
            std::vector<char> str(max_length, 0);
            glGetShaderInfoLog(shader, max_length, &length, &str[0]);
            return &str[0];
        }

        std::string
        GetProgramLog(GLuint shader)
        {
            int length = 0;
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
            if(length <= 0)
            {
                return "";
            }
            const int         max_length = length + 1;
            std::vector<char> str(max_length, 0);
            glGetProgramInfoLog(shader, max_length, &length, &str[0]);
            return &str[0];
        }
    }  // namespace

    void
    ReportError(const std::string& log, const std::string& type)
    {
        LOG_ERROR
        (
            "ERROR::SHADER: Compile-time error: Type: {0} \n"
            "{1}\n"
            "---------------------------------------------------------",
            type,
            core::Trim(log)
        );
    }

    void
    PrintErrorProgram(GLuint program)
    {
        const std::string& log = GetProgramLog(program);
        ReportError(log, "PROGRAM");
    }

    void
    PrintErrorShader(GLuint shader, const std::string& type)
    {
        const std::string& log = GetShaderLog(shader);
        ReportError(log, type);
    }

    GLuint
    CompileShader(GLuint type, const GLchar* source, const std::string& name)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        if(WasCompilationSuccessful(shader))
        {
            return shader;
        }
        else
        {
            PrintErrorShader(shader, name);
            return shader;
        }
    }

    void
    Shader::PreBind(const ShaderAttribute& attribute)
    {
        glBindAttribLocation(GetId(), attribute.id, attribute.name.c_str());
        bound_attributes_.push_back(attribute);
    }

    bool
    Shader::Compile(
            const GLchar* vertex_source,
            const GLchar* fragment_source,
            const GLchar* geometry_source)
    {
        bool ret = true;

        GLuint vertex_shader_id
                = CompileShader(GL_VERTEX_SHADER, vertex_source, "VERTEX");
        GLuint fragment_shader_id = CompileShader(
                GL_FRAGMENT_SHADER, fragment_source, "FRAGMENT");

        GLuint geometry_shader_id = 0;
        if(geometry_source != nullptr)
        {
            geometry_shader_id = CompileShader(
                    GL_GEOMETRY_SHADER, geometry_source, "GEOMETRY");
        }

        glAttachShader(GetId(), vertex_shader_id);
        glAttachShader(GetId(), fragment_shader_id);
        if(geometry_source != nullptr)
        {
            glAttachShader(GetId(), geometry_shader_id);
        }
        glLinkProgram(GetId());
        const bool link_ok = WasLinkingSuccessful(GetId());
        if(!link_ok)
        {
            PrintErrorProgram(GetId());
            ret = false;
        }

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
        if(geometry_source != nullptr)
        {
            glDeleteShader(geometry_shader_id);
        }

        for(const auto& attribute: bound_attributes_)
        {
            int actual_attribute_id
                    = glGetAttribLocation(GetId(), attribute.name.c_str());
            if(actual_attribute_id == attribute.id)
            {
                continue;
            }
            if(actual_attribute_id == -1)
            {
                continue;
            }
            LOG_ERROR
            (
                "{0} was bound to {1} but was requested at {2}",
                attribute.name,
                actual_attribute_id,
                attribute.id
            );
            ret = false;
        }

        return ret;
    }

    ShaderUniform
    Shader::GetUniform(const std::string& name)
    {
        int           uniform_id = glGetUniformLocation(GetId(), name.c_str());
        ShaderUniform uniform(name, uniform_id, this);
        bound_uniforms_.push_back(uniform);

        if(uniform.id == -1)
        {
            LOG_ERROR
            (
                "Failed to load uniform {0} from shader {1}",
                uniform.name,
                shader_name_
            );
        }

        return uniform;
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, float val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        glUniform1f(attribute.id, val);
    }


    void
    Shader::SetUniform(const ShaderUniform& attribute, glint val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        glUniform1i(attribute.id, val);
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::Rgb& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform3f(attribute.id, val.r, val.g, val.b);
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::Rgba& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.r, val.g, val.b, val.a);
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::vec3f& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform3f(attribute.id, val.x, val.y, val.z);
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::vec4f& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.x, val.y, val.z, val.w);
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::mat3f& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniformMatrix3fv(attribute.id, 1, GL_FALSE, val.GetDataPtr());
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::mat4f& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniformMatrix4fv(attribute.id, 1, GL_FALSE, val.GetDataPtr());
    }

    void
    Shader::SetUniform(const ShaderUniform& attribute, const core::Rectf& val)
    {
        ASSERT(IsCurrentlyBound());
        ASSERT(HasBoundUniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.left, val.right, val.bottom, val.top);
    }

    Shader::Shader()
        : shader_name_("~/not_loaded_shader")
    {
    }

    namespace
    {
        std::string
        LoadPath(core::vfs::FileSystem* fs, const core::vfs::FilePath& path)
        {
            // todo: replace with a template instead of basic string
            std::string content;
            if(!fs->ReadFileToString(path, &content))
            {
                return "";
            }

            return content;
        }
    }  // namespace

    bool
    Shader::Load(core::vfs::FileSystem* fs, const core::vfs::FilePath& file_path)
    {
        shader_name_ = file_path;
        auto vert = LoadPath(fs, file_path.SetExtensionCopy("vert"));
        auto frag = LoadPath(fs, file_path.SetExtensionCopy("frag"));
        auto geom = LoadPath(fs, file_path.SetExtensionCopy("geom"));
        bool loaded_files = true;
        if(vert.empty())
        {
            LOG_ERROR("Failed to load vert shader {0}", file_path);
            loaded_files = false;
        }

        if(frag.empty())
        {
            LOG_ERROR("Failed to load frag shader {0}", file_path);
            loaded_files = false;
        }

        if(!loaded_files)
        {
            return false;
        }

        const bool shader_compiled = Compile
        (
            vert.c_str(),
            frag.c_str(),
            geom.empty() ? nullptr : geom.c_str()
        );
        if(!shader_compiled)
        {
            LOG_ERROR("Failed to compile shader {0}", file_path);
        }
        return shader_compiled;
    }

    const std::vector<ShaderAttribute>&
    Shader::GetAttributes() const
    {
        return bound_attributes_;
    }

    const core::vfs::FilePath&
    Shader::GetName() const
    {
        return shader_name_;
    }

    bool
    Shader::HasBoundAttribute(const ShaderAttribute& attribute) const
    {
        return std::find(
                       bound_attributes_.begin(),
                       bound_attributes_.end(),
                       attribute)
               != bound_attributes_.end();
    }

    bool
    Shader::HasBoundUniform(const ShaderUniform& uniform) const
    {
        return std::find(
                       bound_uniforms_.begin(), bound_uniforms_.end(), uniform)
               != bound_uniforms_.end();
    }

    void
    BindTextureToShader(
            Texture2d*           texture,
            Shader*              shader,
            const ShaderUniform& attribute,
            glint                index)
    {
        ASSERT(index < 16);  // at most 16 texture units
        GLenum gl_id = GL_TEXTURE0 + index;
        glActiveTexture(gl_id);
        Use(texture);
        shader->SetUniform(attribute, index);
    }

}  // namespace euphoria::render
