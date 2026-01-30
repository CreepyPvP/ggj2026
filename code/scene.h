#pragma once

#include "types.h"

struct Scene
{
    void (*Frame)(f32 delta);
    void (*Setup)();
    void (*Destroy)();
    f32 time;
};

extern Scene *active_scene;

void SceneStart(Scene *scene);
void SceneFrame(f32 delta);
