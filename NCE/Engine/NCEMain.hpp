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
                ColliderWeakPtrVector m_entityColliders;
                std::vector<NCE::Common::Rect> m_spriteRects;
        };

        //Get rid of this
        struct PlatformProcedures
        {
            processSystemMessagesFunc Win32ProcessSystemMessages;
        };

        void InitializeEngine(int screenWidth_, int screenHeight_, processSystemMessagesFunc processSystemMessages_);
        void InitializeRenderer(NCE::Graphics::Win32DisplayBufferFunc displayBuffer_);

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
        void RegisterEntity(Common::EntitySharedPtr entity_);
        void DestroyEntity(int entityID_);
}


#endif