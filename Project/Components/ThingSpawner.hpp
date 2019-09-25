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
        double m_spawnRate = 0.005;
        double m_currentTime = 0.0;

    public:
        ThingSpawner(NCE::Common::EntityWeakPtr parent_);

        void OnInitialize()  override;
        void OnFrameUpdate() override;

        void SpawnThing();     
};


#endif