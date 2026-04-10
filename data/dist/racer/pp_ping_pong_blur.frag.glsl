#version 330 core

uniform sampler2D u_texture;
uniform bool u_is_horizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 v_tex_coord;

out vec4 o_frag_color;

void main()
{             
    // gets size of single texel
    vec2 tex_offset = 1.0 / textureSize(u_texture, 0);

    // current fragment's contribution
    vec3 result = texture(u_texture, v_tex_coord).rgb * weight[0];


    if(u_is_horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_texture, v_tex_coord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_texture, v_tex_coord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_texture, v_tex_coord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_texture, v_tex_coord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    
    o_frag_color = vec4(result, 1.0);
}