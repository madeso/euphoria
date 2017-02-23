#version 330 core

in vec4 vertex; // <vec2 position, vec2 texCoords>

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
