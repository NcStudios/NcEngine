#ifndef NCE_ENGINE_NCEMAIN
#define NCE_ENGINE_NCEMAIN

#include <vector>
#include <memory>
#include <unordered_map>

#include "../Common/Common.hpp"
#include "../Graphics/Renderer.hpp"
#include "../LevelManagement/LevelManager.hpp"
#include "../Time/Time.hpp"
#include "IDManager.hpp"
#include "../Containers/QuadTree.hpp"


//temp
#include "../Common/Vector.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Collider.hpp"
#include <iostream>

namespace NCE::Engine
{
        typedef std::unordered_map<int, std::shared_ptr<NCE::Common::Entity>> EntityMap;
        typedef std::vector<std::weak_ptr<NCE::Components::Collider>> ColliderWeakPtrVector;

        typedef void(*processSystemMessagesFunc)();

        struct EngineData
        {
            bool isRunning = true;
        };

        struct EngineSystems
        {
            std::unique_ptr<NCE::Graphics::Renderer> renderer;
        };

        struct EntityData
        {
            EntityMap active;
            EntityMap inactive;
            EntityMap awaitingInitialize;
            EntityMap awaitingDestroy;

            ColliderWeakPtrVector GetEntityColliders();
            std::vector<NCE::Common::Rect> GetEntitySprites();

            private:
                ColliderWeakPtrVector _entityColliders;
                std::vector<NCE::Common::Rect> _spriteRects;
        };

        //Get rid of this
        struct PlatformProcedures
        {
            processSystemMessagesFunc Win32ProcessSystemMessages;
        };

        void InitializeEngine(int t_screenWidth, int t_screenHeight, processSystemMessagesFunc t_processSystemMessages);
        void InitializeRenderer(NCE::Graphics::Win32DisplayBufferFunc t_displayBuffer);

        void ForceRender();

        void NCEMain();
        void Exit();
        
        void FrameUpdate();
        void FixedUpdate();
        
        void SendInitializeToEntities();
        void SendFrameUpdateToEntities();
        void SendFixedUpdateToEntities();
        void SendDestroyToEntities();

        Common::EntityWeakPtr CreateEntity();
        void RegisterEntity(Common::EntitySharedPtr t_entity);
        void DestroyEntity(int t_entityID);
}


#endif