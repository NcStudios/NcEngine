#ifndef NCE_H
#define NCE_H

#include <string>
#include "Common.h"
#include "Component.h"
#include "Entity.h"
#include "Group.h"
#include "Input.h"
#include "Time.h"
#include "Transform.h"
#include "Vector.h"

namespace nc
{
    namespace internal { class Engine; }
    
    class NCE
    {
        public:
            NCE(internal::Engine* enginePtr);

            static EntityHandle CreateEntity();
            static EntityHandle CreateEntity(Vector4 rect, bool enableRendering, bool enablePhysics, const std::string& tag);
            static bool DestroyEntity(EntityHandle handle);
            static Entity* GetEntity(EntityHandle handle);
            static Entity* GetEntity(const std::string& tag); //finds first active entity with specified tag, returns nullptr if not found
            static Transform* GetTransformPtr(ComponentHandle handle);

            
        
        private:
            static internal::Engine* m_engine;
    };
} //end namespace nc

#endif