#include "render/shader.h"

#include <iostream>

#include <fstream>
#include <algorithm>


#include "assert/assert.h"
#include "log/log.h"
#include "base/stringutils.h"
#include "io/vfs.h"

#include "render/gl.h"
#include "render/texture.h"


namespace
{


    const eu::render::ShaderProgram*&
    get_current_shader()
    {
        static const eu::render::ShaderProgram* current_shader = nullptr;
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
        glGetShaderInfoLog(shader, max_length, &length, str.data());
        return str.data();
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
        glGetProgramInfoLog(shader, max_length, &length, str.data());
        return str.data();
    }


}


namespace eu::render
{


    ShaderId::ShaderId()
        : id(glCreateProgram())
    {
    }


    ShaderId::~ShaderId()
    {
        glDeleteProgram(id);
    }


    GLuint
    ShaderId::get_id() const
    {
        return id;
    }


    bool
    ShaderId::is_currently_bound() const
    {
        return this == get_current_shader();
    }


    void
    use(const ShaderProgram* shader)
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


    const ShaderProgram*
    ShaderProgram::get_current_bound_for_debug()
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
            trim(log)
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
    ShaderProgram::add_attribute(const ShaderAttribute& attribute)
    {
        const std::string attribute_name = std::string(attribute.name);
        glBindAttribLocation(get_id(), attribute.id, attribute_name.c_str());
        bound_attributes.push_back(attribute);
    }


    bool
    ShaderProgram::compile
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

        for(const auto& attribute: bound_attributes)
        {
            const std::string attribute_name = std::string(attribute.name);
            const int actual_attribute_id = glGetAttribLocation(get_id(), attribute_name.c_str());

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
    ShaderProgram::get_uniform(const std::string& name)
    {
        int uniform_id = glGetUniformLocation(get_id(), name.c_str());
        ShaderUniform uniform(name, uniform_id, this);
        bound_uniforms.push_back(uniform);

        if(uniform.id == -1)
        {
            LOG_ERROR
            (
                "Failed to load uniform {0} from shader {1}",
                uniform.name,
                shader_name
            );
        }

        return uniform;
    }


    void
    ShaderProgram::set_uniform(const ShaderUniform& attribute, float val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        glUniform1f(attribute.id, val);
    }


    void
    ShaderProgram::set_uniform(const ShaderUniform& attribute, gl::Int val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        glUniform1i(attribute.id, val);
    }


    void
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const Rgb& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const Rgba& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const vec3f& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const vec4f& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const mat3f& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const mat4f& val)
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
    ShaderProgram::set_uniform(const ShaderUniform& attribute, const Rectf& val)
    {
        ASSERT(is_currently_bound());
        ASSERT(has_bound_uniform(attribute));
        if(attribute.id == -1)
        {
            return;
        }
        glUniform4f(attribute.id, val.left, val.right, val.bottom, val.top);
    }


    ShaderProgram::ShaderProgram()
        : shader_name("~/not_loaded_shader")
    {
    }


    bool
    ShaderProgram::load(io::FileSystem* fs, const io::FilePath& file_path)
    {
        shader_name = file_path;

        const auto load_path = [](io::FileSystem* afs, const io::FilePath& path) -> std::string
        {
            // todo(Gustav): replace with a template instead of basic string
            auto content = afs->read_file_to_string(path);
            if(!content)
            {
                return "";
            }

            return *content;
        };

        auto vert = load_path(fs, file_path.set_extension_copy("vert"));
        auto frag = load_path(fs, file_path.set_extension_copy("frag"));
        auto geom = load_path(fs, file_path.set_extension_copy("geom"));

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


    const std::vector<ShaderAttribute>&
    ShaderProgram::get_attributes() const
    {
        return bound_attributes;
    }


    const io::FilePath&
    ShaderProgram::get_name() const
    {
        return shader_name;
    }


    bool
    ShaderProgram::has_bound_attribute(const ShaderAttribute& attribute) const
    {
        const auto found = std::find
        (
            bound_attributes.begin(),
            bound_attributes.end(),
            attribute
        );
        return found != bound_attributes.end();
    }


    bool
    ShaderProgram::has_bound_uniform(const ShaderUniform& uniform) const
    {
        const auto found = std::find(bound_uniforms.begin(), bound_uniforms.end(), uniform);
        return found != bound_uniforms.end();
    }


    void
    bind_texture_to_shader
    (
        Texture2* texture,
        ShaderProgram* shader,
        const ShaderUniform& attribute,
        gl::Int index
    )
    {
        ASSERT(index < 16); // at most 16 texture units
        GLenum gl_id = GL_TEXTURE0 + index;
        glActiveTexture(gl_id);
        use(texture);
        shader->set_uniform(attribute, index);
    }


}

