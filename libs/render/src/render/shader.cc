#include "render/shader.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "core/assert.h"
#include "core/log.h"
#include "core/stringutils.h"
#include "core/vfs.h"

#include "render/gl.h"
#include "render/texture.h"

namespace
{
    const euphoria::render::shader*&
    get_current_shader()
    {
        static const euphoria::render::shader* current_shader = nullptr;
        return current_shader;
    }

    bool
    was_compilation_successful(GLuint object)
    {
        int r = GL_TRUE;
        glGetShaderiv(object, GL_COMPILE_STATUS, &r);
        return r == GL_TRUE;
    }

    bool
    was_linking_successful(GLuint object)
    {
        int r = GL_TRUE;
        glGetProgramiv(object, GL_LINK_STATUS, &r);
        return r == GL_TRUE;
    }

    std::string
    get_shader_log(GLuint shader)
    {
        int length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if(length <= 0)
        {
            return "";
        }
        const int max_length = length + 1;
        std::vector<char> str(max_length, 0);
        glGetShaderInfoLog(shader, max_length, &length, &str[0]);
        return &str[0];
    }

    std::string
    get_program_log(GLuint shader)
    {
        int length = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
        if(length <= 0)
        {
            return "";
        }
        const int max_length = length + 1;
        std::vector<char> str(max_length, 0);
        glGetProgramInfoLog(shader, max_length, &length, &str[0]);
        return &str[0];
    }
}


namespace euphoria::render
{
    shader_id::shader_id()
        : id_(glCreateProgram())
    {
    }

    shader_id::~shader_id()
    {
        glDeleteProgram(id_);
    }

    GLuint
    shader_id::get_id() const
    {
        return id_;
    }

    bool
    shader_id::is_currently_bound() const
    {
        return this == get_current_shader();
    }

    void
    use(const shader* shader)
    {
        if(shader != nullptr)
        {
            glUseProgram(shader->get_id());
        }
        else
        {
            glUseProgram(0);
        }
        get_current_shader() = shader;
    }

    const shader*
    shader::get_current_bound_for_debug()
    {
        return get_current_shader();
    }


    void
    report_shader_error(const std::string& log, const std::string& type)
    {
        LOG_ERROR
        (
            "ERROR::SHADER: Compile-time error: Type: {0} \n"
            "{1}\n"
            "---------------------------------------------------------",
            type,
            core::trim(log)
        );
    }

    void
    report_error_program(GLuint program)
    {
        const std::string& log = get_program_log(program);
        report_shader_error(log, "PROGRAM");
    }

    void
    report_error_shader(GLuint shader, const std::string& type)
    {
        const std::string& log = get_shader_log(shader);
        report_shader_error(log, type);
    }

    GLuint
    compile_shader(GLuint type, const GLchar* source, const std::string& name)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        if(was_compilation_successful(shader))
        {
            return shader;
        }
        else
        {
            report_error_shader(shader, name);
            return shader;
        }
    }

    void
    shader::pre_bind(const shader_attribute& attribute)
    {
        glBindAttribLocation(get_id(), attribute.id, attribute.name.c_str());
        bound_attributes_.push_back(attribute);
    }

    bool
    shader::compile
    (
        const GLchar* vertex_source,
        const GLchar* fragment_source,
        const GLchar* geometry_source
    )
    {
        bool ret = true;

        GLuint vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_source, "VERTEX");
        GLuint fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_source, "FRAGMENT");

        GLuint geometry_shader_id = 0;
        if(geometry_source != nullptr)
        {
            geometry_shader_id = compile_shader(GL_GEOMETRY_SHADER, geometry_source, "GEOMETRY");
        }

        glAttachShader(get_id(), vertex_shader_id);
        glAttachShader(get_id(), fragment_shader_id);
        if(geometry_source != nullptr)
        {
            glAttachShader(get_id(), geometry_shader_id);
        }
        glLinkProgram(get_id());
        const bool link_ok = was_linking_successful(get_id());
        if(!link_ok)
        {
            report_error_program(get_id());
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
            const int actual_attribute_id = glGetAttribLocation(get_id(), attribute.name.c_str());

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

    shader_uniform
    shader::get_uniform(const std::string& name)
    {
        int uniform_id = glGetUniformLocation(get_id(), name.c_str());
        shader_uniform uniform(name, uniform_id, this);
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
    shader::set_uniform(const shader_uniform& attribute, float val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        glUniform1f(attribute.id, val);
    }


    void
    shader::set_uniform(const shader_uniform& attribute, glint val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        glUniform1i(attribute.id, val);
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::rgb& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform3f(attribute.id, val.r, val.g, val.b);
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::rgba& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.r, val.g, val.b, val.a);
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::vec3f& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform3f(attribute.id, val.x, val.y, val.z);
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::vec4f& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.x, val.y, val.z, val.w);
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::mat3f& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniformMatrix3fv(attribute.id, 1, GL_FALSE, val.get_data_ptr());
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::mat4f& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniformMatrix4fv(attribute.id, 1, GL_FALSE, val.get_data_ptr());
    }

    void
    shader::set_uniform(const shader_uniform& attribute, const core::rectf& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.left, val.right, val.bottom, val.top);
    }

    shader::shader()
        : shader_name_("~/not_loaded_shader")
    {
    }

    bool
    shader::load(core::vfs::file_system* fs, const core::vfs::file_path& file_path)
    {
        shader_name_ = file_path;

        const auto LoadPath = [](core::vfs::file_system* fs, const core::vfs::file_path& path) -> std::string
        {
            // todo(Gustav): replace with a template instead of basic string
            std::string content;
            if(!fs->read_file_to_string(path, &content))
            {
                return "";
            }

            return content;
        };

        auto vert = LoadPath(fs, file_path.set_extension_copy("vert"));
        auto frag = LoadPath(fs, file_path.set_extension_copy("frag"));
        auto geom = LoadPath(fs, file_path.set_extension_copy("geom"));

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

        const bool shader_compiled = compile
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

    const std::vector<shader_attribute>&
    shader::get_attributes() const
    {
        return bound_attributes_;
    }

    const core::vfs::file_path&
    shader::get_name() const
    {
        return shader_name_;
    }

    bool
    shader::has_bound_attribute(const shader_attribute& attribute) const
    {
        const auto found = std::find
        (
            bound_attributes_.begin(),
            bound_attributes_.end(),
            attribute
        );
        return found != bound_attributes_.end();
    }

    bool
    shader::has_bound_uniform(const shader_uniform& uniform) const
    {
        const auto found = std::find(bound_uniforms_.begin(), bound_uniforms_.end(), uniform);
        return found != bound_uniforms_.end();
    }

    void
    bind_texture_to_shader
    (
        texture2d* texture,
        shader* shader,
        const shader_uniform& attribute,
        glint index
    )
    {
        ASSERT(index < 16); // at most 16 texture units
        GLenum gl_id = GL_TEXTURE0 + index;
        glActiveTexture(gl_id);
        use(texture);
        shader->set_uniform(attribute, index);
    }

}
