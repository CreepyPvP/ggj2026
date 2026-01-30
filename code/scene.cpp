#include "scene.h"

Scene *active_scene;

void SceneStart(Scene *scene)
{
    active_scene = scene;
    active_scene->time = 0;

    if (active_scene->Setup)
        active_scene->Setup();
}

void SceneFrame(f32 delta)
{
    active_scene->Frame();
    active_scene->time += delta;
}
