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

inline f32 EaseOutExpo(f32 x)
{
    f32 t = 1 - x;
    return 1 - t * t * t * t;
}

inline char *FloatToTime(float float_time, char buffer[]) {
    int totalMilliseconds = (int)(float_time * 1000.0f);
    int minutes = (totalMilliseconds % 3600000) / 60000;
    int secs    = (totalMilliseconds % 60000) / 1000;
    int millis  = totalMilliseconds % 1000;

    buffer[0] = '0' + (minutes / 10);
    buffer[1] = '0' + (minutes % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (secs / 10);
    buffer[4] = '0' + (secs % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (millis / 100);
    buffer[7] = '0' + ((millis / 10) % 10);
    buffer[8] = '0' + (millis % 10);
    buffer[9] = '\0';

    return buffer;
}
