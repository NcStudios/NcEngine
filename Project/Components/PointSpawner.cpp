#include "PointSpawner.h"


PointSpawner::PointSpawner(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle) {}

void PointSpawner::OnInitialize()
{
    srand(30);
}

void PointSpawner::FrameUpdate()
{
    if(timeSinceLastSpawn > secondsPerSpawn)
    {
        timeSinceLastSpawn = 0.0f;
        Spawn();
    }
    else
    {
        timeSinceLastSpawn += nc::time::Time::FrameDeltaTime;
    }
    
}

void PointSpawner::Spawn()
{
    int randX = rand() % nc::ProjectSettings::displaySettings.screenWidth;
    int randY = rand() % nc::ProjectSettings::displaySettings.screenHeight;

    nc::EntityHandle h = NCE::CreateEntity(Vector4(randX, randY, pointSize, pointSize), true, true, "");
    NCE::GetEntity(h)->AddComponent<Character2>();
}