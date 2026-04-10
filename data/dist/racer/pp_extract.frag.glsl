#version 330 core

uniform sampler2D u_texture;

uniform float u_cutoff;
uniform float u_softness;

in vec2 v_tex_coord;

out vec4 o_frag_color;


// https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/
float calculate_contribution(float brightness)
{
    float threshold = u_cutoff;
    float knee = threshold * u_softness;
    float x = clamp(brightness - threshold + knee, 0, 2 * knee);
	float soft = (x*x) / (4 * knee + 0.00001f);
	float contribution = max(soft, brightness - threshold) / max(brightness, 0.00001f);
    return contribution;
}

void main()
{
    vec4 sampled = texture(u_texture, v_tex_coord);
    float alpha = sampled.a;
    vec3 color = sampled.rgb;

    // calculates "relative luminance" from linear color space: https://en.wikipedia.org/wiki/Relative_luminance
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

	color = color * calculate_contribution(brightness);

    o_frag_color = vec4(color, alpha);
}
