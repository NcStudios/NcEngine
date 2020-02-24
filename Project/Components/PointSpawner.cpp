#include "PointSpawner.h"

#include <iostream>

PointSpawner::PointSpawner(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle) {}

void PointSpawner::OnInitialize()
{
    srand(30);
}

void PointSpawner::FrameUpdate()
{
    if(!doesPointExist)
    {
        Spawn();
    }
}

void PointSpawner::PointConsumed()
{
    doesPointExist = false;
}

void PointSpawner::Spawn()
{
    doesPointExist = true;
    int randX = rand() % nc::ProjectSettings::displaySettings.screenWidth;
    int randY = rand() % nc::ProjectSettings::displaySettings.screenHeight;

    nc::EntityHandle h = NCE::CreateEntity(Vector4(randX, randY, pointSize, pointSize), true, true, "");
    auto point = NCE::GetEntity(h)->AddComponent<Point>();
    point->SpawnerHandle = GetEntityHandle();
}