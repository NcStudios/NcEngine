#include "NCEMain.hpp"

namespace NCE::Engine
{
    ColliderWeakPtrVector EntityData::GetEntityColliders()
    {
        m_entityColliders.clear();
        std::weak_ptr<NCE::Components::Collider> pCollider;

        for (auto pair : active)
        {
            pCollider = pair.second->GetComponent<NCE::Components::Collider>(7);
            if (pCollider.lock() == nullptr)
            {
                continue;
            }

            m_entityColliders.push_back(pCollider);
        }

        return m_entityColliders;
    }

    std::vector<NCE::Common::Rect> EntityData::GetEntitySprites()
    {
        m_spriteRects.clear();
        std::weak_ptr<NCE::Components::Transform> transform;

        for (auto pair : active)
        {
            transform = pair.second->GetComponent<NCE::Components::Transform>(1);
            if (transform.lock() == nullptr)
            {
                continue;
            }

            m_spriteRects.push_back(transform.lock()->GetRect());
        }

        return m_spriteRects;
    }

    const int FPS = 60;
    const float FIXED_UPDATE_INTERVAL = 0.05;


    PlatformProcedures platformProcedures;
    EngineData engineData;
    EngineSystems engineSystems;
    EntityData entityData;
    IDManager idManager;
    NCE::LevelManagement::LevelManager levelManager(CreateEntity);

    //may chagne - dimensions hard coded in Win32 layer
    NCE::Common::Vector2 screenDimensions;

    std::vector<NCE::Common::Rect> entitySprites;
    std::vector<std::weak_ptr<NCE::Components::Collider>> entityColliders;

    NCE::Time nceTime;
    const uint64_t MICRO_SECONDS_PER_FRAME = 1000000 / FPS;
    const uint64_t MICRO_SECONDS_PER_FIXED = 1000000 * FIXED_UPDATE_INTERVAL;
    uint64_t microSecondsSinceLastCycle = 0; 
    uint64_t microSecondsSinceLastFrame = 0;
    uint64_t microSecondsSinceLastFixed = 0;

    NCE::Common::Vector4 worldSpaceRect(0, 0, 0, 0);
    

    void InitializeEngine(int screenWidth_, int screenHeight_, processSystemMessagesFunc processSystemMessages_)
    {
        screenDimensions.Set(screenWidth_, screenHeight_);
        worldSpaceRect.Set(0, 0, screenWidth_, screenHeight_);

        platformProcedures.Win32ProcessSystemMessages = processSystemMessages_;
        //platformProcedures.render = t_render;

        levelManager.CreateTestLevel();
    }

    void InitializeRenderer(NCE::Graphics::Win32DisplayBufferFunc displayBuffer_)
    {
        engineSystems.renderer = std::make_unique<NCE::Graphics::Renderer>(displayBuffer_, screenDimensions.GetX(), screenDimensions.GetY());
    }

    void ForceRender()
    {
        //need null check?
        engineSystems.renderer->ForceRender();
    }

    void FrameUpdate()
    {
        //Frame Updates for GameObject/Component instances
        SendInitializeToEntities();
        SendFrameUpdateToEntities();
        SendDestroyToEntities();

        //Render
        entitySprites = entityData.GetEntitySprites();
        engineSystems.renderer->StartRenderCycle(entitySprites);
        entitySprites.clear();

        //Cleanup
        NCE::Input::Flush();
        microSecondsSinceLastFrame = 0;
    }

    void FixedUpdate()
    {        
        NCE::Containers::QuadTree colliderQuadTree(4, worldSpaceRect);

        entityColliders = entityData.GetEntityColliders();
        
        for (auto colliderPtr : entityColliders)
        {
            colliderQuadTree.AddElement(colliderPtr);
        }

        colliderQuadTree.CheckCollisions();

        for(auto colliderPtr : entityColliders)
        {
            if (colliderPtr.expired())
            {
                //why is this never reached??
                std::cout << "colliderPtr expired" << '\n';
            }
            else
            {
                colliderPtr.lock()->NewPhysicsCycle();
            }
        }

        SendFixedUpdateToEntities();

        //colliderQuadTree.Clear();
        //entityColliders.clear();
        microSecondsSinceLastFixed = 0;
    }

    void NCEMain()
    {
        while(engineData.isRunning)
        {
            //Time
            nceTime.UpdateTime();
            microSecondsSinceLastCycle = NCE::Time::GetDeltaTime();
            microSecondsSinceLastFrame += microSecondsSinceLastCycle;
            microSecondsSinceLastFixed += microSecondsSinceLastCycle;

            //System Input
            platformProcedures.Win32ProcessSystemMessages();

            //Physics and Fixed Interval Update
            if (microSecondsSinceLastFixed > MICRO_SECONDS_PER_FIXED)
            {
                FixedUpdate();
            } 

            //Frame Update
            if (microSecondsSinceLastFrame > MICRO_SECONDS_PER_FRAME)
            {
                FrameUpdate();
            } 
        }
    }

    void Exit()
    {
        engineData.isRunning = false;
    }

    
    void SendInitializeToEntities()
    {
        for (auto& pair : entityData.awaitingInitialize)
        {
            pair.second->SendInitializeToComponents();
            entityData.active.insert({pair.second->GetID(), pair.second});
        }

        entityData.awaitingInitialize.clear();
    }

    void SendFrameUpdateToEntities()
    {
        for(auto& pair : entityData.active)
        {
            pair.second->SendFrameUpdateToComponents();
        }
    }

    void SendFixedUpdateToEntities()
    {
        for(auto& pair : entityData.active)
        {
            pair.second->SendFixedUpdateToComponents();
        }
    }

    void SendDestroyToEntities()
    {
        for(auto& pair : entityData.awaitingDestroy)
        {
            pair.second->SendDestroyToComponents();
        }

        entityData.awaitingDestroy.clear();
    }

    Common::EntityWeakPtr CreateEntity()
    {
        int newEntityID = idManager.GetNewEntityID();
        auto newEntity = std::make_shared<NCE::Common::Entity>(newEntityID, CreateEntity, DestroyEntity);
        newEntity->AddComponent<NCE::Components::Transform>(newEntity);
        RegisterEntity(newEntity);
        return newEntity;
    }

    void RegisterEntity(Common::EntitySharedPtr entity_)
    {
        if (entity_ == nullptr)
        {
            std::cout << "NCEMain.RegisterEntity() - t_entity is NULL" << '\n';
        }
        
        entityData.awaitingInitialize.insert({entity_->GetID(), entity_});
    }

    void DestroyEntity(int entityID_)
    {
        int found = entityData.active.count(entityID_);

        if (found)
        {
            auto entity = entityData.active.at(entityID_);
            entityData.awaitingDestroy.insert( {entityID_, entity} );
            entityData.active.erase(entityID_);
        }
    }

}