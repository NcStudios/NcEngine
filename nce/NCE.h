#pragma once
#include <string>
#include "Common.h"
#include "Component.h"
#include "Entity.h"
#include "Input.h"
#include "NCTime.h"
#include "Transform.h"
#include "NCVector.h"

namespace nc
{
    namespace internal { class Engine; }
    
    class NCE
    {
        public:
            NCE(internal::Engine* enginePtr);

            static void Exit();

            static EntityHandle CreateEntity();
            static EntityHandle CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics, const std::string& tag);
            static bool DestroyEntity(EntityHandle handle);
            static Transform* GetTransformPtr(ComponentHandle handle);

            static Entity* GetEntity(EntityHandle handle);
            static Entity* GetEntity(const std::string& tag); //finds first active entity with specified tag, returns nullptr if not found

        private:
            static internal::Engine* m_engine;
    };

} //end namespace nc