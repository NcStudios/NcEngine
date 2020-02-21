#ifndef PointSpawner_H
#define PointSpawner_H

#include "../../NCE/include/NCE.h"
#include "../../NCE/include/ProjectSettings.h"
#include "Character2.h"

#include <iostream>
#include <random>

using namespace nc;

class PointSpawner : public Component
{
    private:
        const float secondsPerSpawn = 0.5f;
        float timeSinceLastSpawn = 0.0f;
        int screenWidth = 0;
        int screenHeight = 0;
        int pointSize = 32;

    public:
        PointSpawner(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize()  override;
        void FrameUpdate() override;   

        void Spawn();
};


#endif