#pragma once
#include <string>
#include "Common.h"
#include "Component.h"
#include "Entity.h"
#include "View.h"
#include "Input.h"
#include "NCTime.h"
#include "Transform.h"
#include "NCVector.h"
#include "Renderer.h"

namespace nc
{
    class Camera;
    namespace engine { class Engine; }
    
    class NCE
    {
        public:
            NCE(engine::Engine* enginePtr);

            static graphics::Graphics& GetGraphics();
            static void Exit();

            static EntityView* GetMainCamera();
            static EntityView  CreateEntity();
            static EntityView  CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag);
            static EntityView  GetEntityView(EntityHandle handle);
            static Entity*     GetEntity(EntityHandle handle);
            static Entity*     GetEntity(const std::string& tag); //finds first active entity with specified tag, returns nullptr if not found
            static bool        DestroyEntity(EntityHandle handle);
            static Transform*  GetTransform(ComponentHandle handle);
            static Renderer*   AddRenderer(EntityHandle handle);
            static Renderer*   GetRenderer(EntityHandle handle);
            static bool        RemoveRenderer(EntityHandle handle);

        private:
            static engine::Engine* m_engine;
    };

    

} //end namespace nc