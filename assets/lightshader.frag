#version 330

in vec2 fragTexCoord;

out vec4 final_color;

uniform sampler2D color_buffer;
uniform sampler2D light_buffer;

uniform vec2 size;

bool DeadzoneColor(vec3 color, vec3 comp)
{
    return  color.x >= comp.x - 0.11 && color.x <= comp.x + 0.11 &&
            color.y >= comp.y - 0.11 && color.y <= comp.y + 0.11 &&
            color.z >= comp.z - 0.11 && color.z <= comp.z + 0.11;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / size;

    vec3 color_sample = texture(color_buffer, uv).rgb;
    vec3 light_sample = texture(light_buffer, uv).rgb;

    if (length(light_sample) > 0.3)
    {
        final_color = vec4(light_sample, 1);
        if (DeadzoneColor(color_sample, vec3(24, 20, 37) / 256.0f))
        {
            final_color = vec4(color_sample, 1);
        }
    }
    else
    {
        final_color = vec4(color_sample, 1);
    }
}

