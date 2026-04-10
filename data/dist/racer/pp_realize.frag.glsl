#version 330 core

uniform sampler2D u_texture;
uniform sampler2D u_blurred_bloom;

uniform bool u_use_blur;
uniform float u_gamma;
uniform float u_exposure;

in vec2 v_tex_coord;

out vec4 o_frag_color;

void main()
{
    vec4 sampled = texture(u_texture, v_tex_coord);
    float alpha = sampled.a;
    vec3 color = sampled.rgb;

    if(u_use_blur)
    {
        vec3 blur = texture(u_blurred_bloom, v_tex_coord).rgb;
        color += blur;
    }

    if(u_exposure > 0.0f)
    {
        // todo(Gustav): add more tonemapping
        // todo(Gustav): make tone mapping configurable?
        // reinhard tone mapping
        // color = color / (color + vec3(1.0f));

        // todo(Gustav): what is the name of this?
        // exposure tone mapping
        color = vec3(1.0f) - exp(-color * u_exposure);
    }
    vec3 corrected = pow(color.rgb, vec3(1.0f/u_gamma));
    o_frag_color = vec4(corrected, alpha);
}
