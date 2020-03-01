#ifndef CHARACTER2
#define CHARACTER2

#include "NCE.h"
#include "Head.h"
//#include "PointSpawner.h"

using namespace nc;

class Point : public Component
{
    public:
        Point(ComponentHandle handle, EntityHandle parentHandle);

        void OnCollisionEnter(const EntityHandle other) override;

        EntityHandle SpawnerHandle;
};
 

#endif