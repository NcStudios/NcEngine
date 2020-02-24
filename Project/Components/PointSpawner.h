#ifndef PointSpawner_H
#define PointSpawner_H

#include "../../NCE/include/NCE.h"
#include "../../NCE/include/ProjectSettings.h"
#include "Point.h"

#include <iostream>
#include <random>

using namespace nc;

class PointSpawner : public Component
{
    private:
        //const float secondsPerSpawn = 0.5f;
        //float timeSinceLastSpawn = 0.0f;
        bool doesPointExist;
        int screenWidth = 0;
        int screenHeight = 0;
        int pointSize = 32;

    public:
        PointSpawner(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize()  override;
        void FrameUpdate() override;   

        void PointConsumed();
        void Spawn();
};


#endif