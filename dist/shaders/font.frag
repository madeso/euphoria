#version 330 core

in vec2 TexCoords;
out vec4 outColor;

uniform sampler2D image;

uniform vec3 color;
void main()
{
    outColor = vec4(color, 1.0) * texture(image, TexCoords);
}
