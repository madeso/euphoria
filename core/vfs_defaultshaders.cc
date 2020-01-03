#include "core/vfs_defaultshaders.h"

#include "core/stringutils.h"
#include "core/log.h"
#include "core/vfs_path.h"

namespace euphoria::core
{
    namespace vfs
    {
        void
        AddDefaultShaders(FileSystem* fs, const DirPath& base)
        {
            auto cat = FileSystemRootCatalog::AddRoot(fs);
            cat->RegisterFileString(
                    base.GetFile("sprite.vert"),
                    R"STRING(
                    #version 330 core

                    in vec4 vertex; // <vec2 position, vec2 texCoords>

                    out vec2 TexCoords;

                    uniform mat4 model;
                    uniform mat4 projection;

                    uniform vec4 color;
                    uniform vec4 region;

                    void main()
                    {
                        TexCoords = vec2(region.x + vertex.z * (region.y - region.x),
                                        region.z + vertex.w * (region.w - region.z));
                        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
                    }
                )STRING");
            cat->RegisterFileString(
                    base.GetFile("sprite.frag"),
                    R"STRING(
                    #version 330 core

                    in vec2 TexCoords;
                    out vec4 outColor;

                    uniform sampler2D image;
                    uniform vec4 color;

                    void main()
                    {
                        outColor = color * texture(image, TexCoords);
                    }
                )STRING");
        }

    }  // namespace vfs

}  // namespace euphoria::core
