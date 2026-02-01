#version 330

in vec2 fragTexCoord;

out vec4 final_color;

uniform sampler2D color_buffer;
uniform sampler2D light_buffer;

uniform vec2 size;

bool DeadzoneColor(vec3 color, vec3 comp)
{
    return  color.x >= comp.x - 0.001 && color.x <= comp.x + 0.001 &&
            color.y >= comp.y - 0.001 && color.y <= comp.y + 0.001 &&
            color.z >= comp.z - 0.001 && color.z <= comp.z + 0.001;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / size;

    vec3 color_sample = texture(color_buffer, uv).rgb;
    vec3 light_sample = texture(light_buffer, uv).rgb;

    float luminance = dot(color_sample, vec3(0.2125, 0.7154, 0.0721));

    if (length(light_sample) > 0.3)
    {
        float brighness = mix((1 - luminance), 1, 0.5);

        final_color = vec4(brighness * light_sample, 1);
        // final_color = vec4(light_sample, 1);
        if (DeadzoneColor(color_sample, vec3(24, 20, 37) / 256.0f))
        {
            final_color = vec4(1, 1, 1, 1);
        }
    }
    else
    {
        final_color = vec4(color_sample, 1);
    }
}

