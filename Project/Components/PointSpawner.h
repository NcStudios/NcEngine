#ifndef PointSpawner_H
#define PointSpawner_H

#include "../../NCE/Include/NCE.h"
#include "../../NCE/Include/Component.h"
//#include "../../NCE/Include/Time.h"
#include "../../NCE/Include/Transform.h"
#include "../../NCE/Include/Vector.h"

#include <iostream>

using namespace nc;

class PointSpawner : public Component
{
    private:
        const int numberOfPoints = 100000;

        Vector2 upperBound;
        Vector2 lowerLeftBound;
        Vector2 lowerRightBound;
        
        Vector2 GetBoundPosition(const int boundIndex) const;

    public:
        PointSpawner(ComponentHandle handle, EntityHandle parentHandle);

        void OnInitialize()  override;

        void InitializeBounds();
        void SpawnPoints();     
};


#endif