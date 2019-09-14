#ifndef THINGSPAWNER
#define THINGSPAWNER

#include <memory>
#include <time.h>
#include <cstdlib>

#include "../../NCE/Common/Common.hpp"
#include "Character2.hpp"
#include "../../NCE/Time/Time.hpp"
#include "../../NCE/Components/Collider.hpp"


class ThingSpawner : public NCE::Common::Component
{
    private:
        double _spawnRate = 0.005;
        double _currentTime = 0.0;

    public:
        ThingSpawner(NCE::Common::EntityWeakPtr t_parent);

        void OnInitialize()  override;
        void OnFrameUpdate() override;

        void SpawnThing();     
};


#endif