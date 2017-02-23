#version 330 core

in vec2 TexCoords;
out vec4 outColor;

uniform sampler2D image;
uniform vec4 color;

void main()
{
    outColor = color * texture(image, TexCoords);
}
