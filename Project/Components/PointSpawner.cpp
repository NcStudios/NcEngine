#include "PointSpawner.h"


PointSpawner::PointSpawner(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle) {}

void PointSpawner::OnInitialize()
{
    ProjectConfig config = NCE::GetProjectConfig();
    screenWidth  = config.ScreenHeight;
    screenHeight = config.ScreenHeight;
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
    int randX = rand() % screenWidth;
    int randY = rand() % screenHeight;

    nc::EntityHandle h = NCE::CreateEntity(Vector4(randX, randY, pointSize, pointSize), true, true);
    NCE::GetEntityPtr(h)->AddComponent<Character2>();
}