#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <random>

#include "NCE.h"
#include "Component.h"
#include "Entity.h"
#include "Transform.h"
#include "Vector.h"

//project includes
#include "../../Project/Components/PointSpawner.h"

namespace nc::scene
{
    Vector4 GetRandomVector4();

    class SceneManager
    {
        private:

        public:
            SceneManager();
            void CreateTestLevel();
    };
} //end namespace nc::scene


#endif