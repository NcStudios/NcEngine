#include "InitialScene.h"

void InitialScene::Load()
{
    EntityHandle thingSpawnerHandle = NCE::CreateEntity();
    NCE::GetEntity(thingSpawnerHandle)->AddComponent<PointSpawner>();

    EntityHandle characterHandle = NCE::CreateEntity(Vector4(50, 50, 64, 64), true, true, "Head");
    NCE::GetEntity(characterHandle)->AddComponent<Head>();
}

void InitialScene::Unload()
{
    
}