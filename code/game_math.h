#pragma once

inline f32 Min(f32 a, f32 b)
{
    return a < b ? a : b;
}

inline f32 Max(f32 a, f32 b)
{
    return a > b ? a : b;
}

inline f32 Range(f32 x, f32 start, f32 end)
{
    return Max(Min((x - start) / (end - start), 1), 0);
}
