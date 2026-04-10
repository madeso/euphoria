#version 330 core

///////////////////////////////////////////////////////////////////////////////
// uniforms

{{^only_depth}}
struct Material
{
    vec4 diffuse_tint; // diffuse + alpha
{{#use_texture}}
    sampler2D diffuse_tex;
{{/use_texture}}

    {{#use_lights}}
    sampler2D specular_tex;
    sampler2D emissive_tex;
    float emissive_factor;
    vec3 ambient_tint;
    vec3 specular_tint;
    float shininess;
    {{/use_lights}}
};

uniform Material u_material;
{{/only_depth}}

{{#use_lights}}
struct DirectionalLight
{
    vec3 diffuse;
    vec3 specular;

    vec3 dir;
};

struct PointLight
{
    vec3 diffuse;
    vec3 specular;
    vec4 attenuation; // min max s t

    vec3 world_pos;
};

struct FrustumLight
{
    vec3 diffuse;
    vec3 specular;
    vec4 attenuation; // min max s t

    mat4 clip_from_world;
    vec3 world_pos; // for specular calc

    sampler2D cookie;
};

uniform sampler2D u_directional_light_depth_tex;

uniform vec3 u_view_position;
uniform vec3 u_ambient_light;

uniform PointLight u_point_lights[{{number_of_point_lights}}];
uniform DirectionalLight u_directional_lights[{{number_of_directional_lights}}];
uniform FrustumLight u_frustum_lights[{{number_of_frustum_lights}}];
{{/use_lights}}


///////////////////////////////////////////////////////////////////////////////
// varying
{{^only_depth}}
in vec3 v_color;
in vec2 v_tex_coord;
{{/only_depth}}

{{#use_lights}}
in vec3 v_worldspace;
in vec3 v_normal;
in vec4 v_directional_shadow_clip_position;
{{/use_lights}}


///////////////////////////////////////////////////////////////////////////////
// output
out vec4 o_frag_color;

///////////////////////////////////////////////////////////////////////////////
// s curve
{{#use_lights}}

float calculate_s_curve(float x, float s, float t)
{
	float mE = 0.00001f;	// machine epsilon
	return x < t ? t * x / (x + s * (t - x) + mE)
				 : ((1 - t) * (x - 1)) / (1 - x - s * (t - x) + mE) + 1;
}

float calculate_attenuation(vec4 att, float distance)
{
    float min_dist = att.x;
    float max_dist = att.y;
    float scale = (distance - min_dist) / (max_dist - min_dist); // 0 at min, 1 at max
    float attenuation = calculate_s_curve(1.0 - clamp(scale, 0, 1), att.z, att.w);
    return attenuation; // 1 at min, 0 at max
}

float calculate_specular(vec3 view_direction, vec3 light_direction, vec3 normal, float shininess)
{
    const float kPi = 3.14159265;

    {{#use_blinn_phong}}
        float energy_conservation = ( 8.0 + shininess ) / ( 8.0 * kPi ); 
        vec3 halfwayDir = normalize(light_direction + view_direction); 
        return energy_conservation * pow(max(dot(normal, halfwayDir), 0.0), shininess);
    {{/use_blinn_phong}}

    {{^use_blinn_phong}}
        float energy_conservation = ( 2.0 + shininess ) / ( 2.0 * kPi ); 
        vec3 reflectDir = reflect(-light_direction, normal);
        return energy_conservation * pow(max(dot(view_direction, reflectDir), 0.0), shininess);
    {{/use_blinn_phong}}
}

vec3 calculate_directional_light(
    DirectionalLight pl, vec3 normal, vec3 view_direction, vec3 spec_t, vec3 base_color)
{
    vec3 light_direction = -pl.dir;
    vec3 reflect_direction = reflect(-light_direction, normal);

    // diffuse color
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_color = diff * (u_material.diffuse_tint.rgb * base_color * pl.diffuse);

    // specular color
    float spec = calculate_specular(view_direction, light_direction, normal, u_material.shininess);
    vec3 specular_color = spec * (u_material.specular_tint * spec_t * pl.specular);

    float shadow = 0.0f;
    // transform from homogenous clip space to NDC
    vec3 directional_shadow_ndc_position = v_directional_shadow_clip_position.xyz / v_directional_shadow_clip_position.w;
    if(directional_shadow_ndc_position.z <= 1.0f)
    {
        // transform from (-1 to +1) to (0 to 1) range
        float max_bias = 0.025f;
        float min_bias = 0.0005f;
        vec3 light_coords = (directional_shadow_ndc_position + 1.0f)/ 2.0f;
        float current_depth = light_coords.z;
        float bias = max(max_bias * (1.0f - dot(normal, light_direction)), min_bias); // todo(Gustav): make tweakable?

        int sample_radius = 2;
        vec2 pixel_size = 1.0 / textureSize(u_directional_light_depth_tex, 0);

        for(int y = -sample_radius; y <= sample_radius; y+=1)
        {
            for(int x = -sample_radius; x <= sample_radius; x+=1)
            {
                float closest_depth = texture(u_directional_light_depth_tex, light_coords.xy + vec2(x, y) * pixel_size).r;
                if(current_depth > (closest_depth + bias))
                {
                    shadow += 1.0f;
                }
            }
        }

        shadow = shadow / pow((sample_radius * 2 + 1), 2);
    }
    float shadow_tint = 1-shadow;

    return (diffuse_color + specular_color)*shadow_tint;
}

vec3 calculate_point_light(
    PointLight pl, vec3 normal, vec3 view_direction, vec3 spec_t, vec3 base_color)
{
    vec3 light_direction = normalize(pl.world_pos - v_worldspace);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // diffuse color
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_color = diff * (u_material.diffuse_tint.rgb * base_color * pl.diffuse);

    // specular color
    float spec = calculate_specular(view_direction, light_direction, normal, u_material.shininess);
    vec3 specular_color = spec * (u_material.specular_tint * spec_t * pl.specular);

    // attenuation
    float distance = length(pl.world_pos - v_worldspace);
    float attenuation = calculate_attenuation(pl.attenuation, distance);

    return (diffuse_color + specular_color) * attenuation;
}

float extract_frustum_light_factor(vec2 uv)
{
    // todo(Gustav): remove branching
    if(uv.x < -1.0) { return 0.0; }
    if(uv.x > 1.0) { return 0.0; }
    if(uv.y < -1.0) { return 0.0; }
    if(uv.y > 1.0) { return 0.0; }
    return 1.0;
}

vec3 calculate_frustum_light(
    FrustumLight pl, vec3 normal, vec3 view_direction, vec3 spec_t, vec3 base_color)
{
    vec3 light_direction = normalize(pl.world_pos - v_worldspace);
    vec3 reflect_direction = reflect(-light_direction, normal);

    vec4 clip_coord = pl.clip_from_world * vec4(v_worldspace, 1.0);
    vec2 ndc = clip_coord.xy / clip_coord.w;
    // look up cookie texture and transform [-1, 1] ndc to [0, 1] uv
    float cookie = texture(pl.cookie, (ndc.xy / 2.0) + 0.5).r;

    float diff = max(dot(normal, light_direction), 0.0);

    float factor = diff * extract_frustum_light_factor(ndc) * cookie;

    // diffuse color
    vec3 diffuse_color = factor * (u_material.diffuse_tint.rgb * base_color * pl.diffuse);

    // specular color
    float spec = calculate_specular(view_direction, light_direction, normal, u_material.shininess);
    vec3 specular_color = factor * spec * (u_material.specular_tint * spec_t * pl.specular);

    // attenuation
    float distance = length(pl.world_pos - v_worldspace);
    float attenuation = calculate_attenuation(pl.attenuation, distance);

    return (diffuse_color + specular_color) * attenuation;
}
{{/use_lights}}

///////////////////////////////////////////////////////////////////////////////
// code
void main()
{
{{^only_depth}}
{{#use_lights}}
    vec3 normal = normalize(v_normal);
    vec3 view_direction = normalize(u_view_position - v_worldspace);
    vec4 tex = texture(u_material.diffuse_tex, v_tex_coord);
    vec3 spec_t = texture(u_material.specular_tex, v_tex_coord).rgb;
    vec3 emi_t = texture(u_material.emissive_tex, v_tex_coord).rgb;
    vec3 base_color = tex.rgb * v_color.rgb;
    float alpha = tex.a * u_material.diffuse_tint.a;

{{#transparent_cutoff}}
    if(alpha < 0.1)
    {
        discard;
    }
    alpha = 1.0;
{{/transparent_cutoff}}

    // ambient color
    vec3 ambient_color = u_material.ambient_tint * base_color * u_ambient_light;

    // emissive color
    vec3 emissive_color = u_material.emissive_factor * emi_t;

    vec3 light_color = ambient_color + emissive_color;

    // directional lights
    for(int i=0; i<{{number_of_directional_lights}}; i+=1)
    {
        light_color += calculate_directional_light(u_directional_lights[i], normal, view_direction, spec_t, base_color);
    }

    // point lights
    for(int i=0; i<{{number_of_point_lights}}; i+=1)
    {
        light_color += calculate_point_light(u_point_lights[i], normal, view_direction, spec_t, base_color);
    }

    // frustum lights
    for(int i=0; i<{{number_of_frustum_lights}}; i+=1)
    {
        light_color += calculate_frustum_light(u_frustum_lights[i], normal, view_direction, spec_t, base_color);
    }

    o_frag_color = vec4(light_color.rgb, alpha);
{{/use_lights}}
{{^use_lights}}
{{#use_texture}}
    vec4 object_color = texture(u_material.diffuse_tex, v_tex_coord)
        * u_material.diffuse_tint.rgba * vec4(v_color.rgb, 1.0);
{{#transparent_cutoff}}
    if(object_color.a < 0.1)
    {
        discard;
    }
    object_color.a = 1.0;
{{/transparent_cutoff}}
    o_frag_color = object_color;
{{/use_texture}}
{{^use_texture}}
    vec4 object_color = u_material.diffuse_tint.rgba * vec4(v_color.rgb, 1.0);
    o_frag_color = object_color;
{{/use_texture}}
{{/use_lights}}
{{/only_depth}}
}
