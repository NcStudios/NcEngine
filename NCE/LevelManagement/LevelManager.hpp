#ifndef NCE_LEVELMANAGEMENT_LEVELMANAGER
#define NCE_LEVELMANAGEMENT_LEVELMANAGER

#include <memory>

#include "../Engine/NCEMain.hpp"
#include "../Common/Common.hpp"


//for setting up test level
#include "../Components/Transform.hpp"
#include "../Components/Collider.hpp"
#include "../../Project/Components/Character.hpp"
#include "../../Project/Components/ThingSpawner.hpp"

namespace NCE::LevelManagement
{
    typedef NCE::Common::EntityWeakPtr (*createEntityFunc)();

    class LevelManager
    {
        private:
            createEntityFunc CreateEntity;

        public:
            LevelManager(createEntityFunc createEntityFunc_);
            void CreateTestLevel();
    };
}

#endif