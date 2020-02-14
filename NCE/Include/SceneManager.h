#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include "NCE.h"
#include "Component.h"
#include "Entity.h"
#include "Transform.h"
#include "Vector.h"
#include <random>

#include "../../Project/Components/PointSpawner.h"

Vector4 GetRandomVector4();

class SceneManager
{
    private:

    public:
        SceneManager();
        void CreateSampleLevel();
        void CreateTestLevel();
};

#endif