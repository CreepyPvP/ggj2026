

#include "render_util.h"


static void GameDrawCone(Vector2 pos, f32 forward_angle, f32 length, f32 angle)
{
    u32 sample_points = 128;

    f32 start_angle = forward_angle + angle / 2;
    f32 end_angle = forward_angle - angle / 2;

    Vector2 prev_sample;

    {
        Vector2 current_dir = {cos(start_angle / 180.0f * PI), sin(start_angle / 180.0f * PI)};
        f32 len = Min(GameRaycast(pos, current_dir), length);
        prev_sample = (pos + current_dir * len) * 32;
    }

    for (u32 i = 1; i < sample_points; ++i)
    {
        f32 current_angle = start_angle + (end_angle - start_angle) * ((f32) i / ((f32) sample_points - 1));
        Vector2 current_dir = {cos(current_angle / 180.0f * PI), sin(current_angle / 180.0f * PI)};

        f32 len = Min(GameRaycast(pos, current_dir), length);
        Vector2 sample = (pos + current_dir * len) * 32;

        DrawTriangle(pos * 32, prev_sample, sample, Fade(BLUE, 0.5));
        prev_sample = sample;
    }
}
