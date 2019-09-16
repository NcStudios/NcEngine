#include "NCEMain.hpp"



namespace NCE::Engine
{
    ColliderWeakPtrVector EntityData::GetEntityColliders()
    {
        _entityColliders.clear();
        std::weak_ptr<NCE::Components::Collider> pCollider;

        for (auto pair : active)
        {
            pCollider = pair.second->GetComponent<NCE::Components::Collider>(7);
            if (pCollider.lock() == nullptr)
            {
                continue;
            }

            _entityColliders.push_back(pCollider);
        }

        return _entityColliders;
    }

    std::vector<NCE::Common::Rect> EntityData::GetEntitySprites()
    {
        _spriteRects.clear();
        std::weak_ptr<NCE::Components::Transform> pTransform;

        for (auto pair : active)
        {
            pTransform = pair.second->GetComponent<NCE::Components::Transform>(1);
            if (pTransform.lock() == nullptr)
            {
                continue;
            }

            _spriteRects.push_back(pTransform.lock()->GetRect());
        }

        return _spriteRects;
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
    

    void InitializeEngine(int t_screenWidth, int t_screenHeight, processSystemMessagesFunc t_processSystemMessages)
    {
        screenDimensions.Set(t_screenWidth, t_screenHeight);
        worldSpaceRect.Set(0, 0, t_screenWidth, t_screenHeight);

        platformProcedures.Win32ProcessSystemMessages = t_processSystemMessages;
        //platformProcedures.render = t_render;

        levelManager.CreateTestLevel();
    }

    void InitializeRenderer(NCE::Graphics::Win32DisplayBufferFunc t_displayBuffer)
    {
        std::cout << "start InitializeRenderer()" << '\n';
        engineSystems.renderer = std::make_unique<NCE::Graphics::Renderer>(t_displayBuffer, screenDimensions.GetX(), screenDimensions.GetY());
        std::cout << "end InitializeRenderer()" << '\n';
    }

    void ForceRender()
    {
        //need null check?
        engineSystems.renderer->ForceRender();
    }

    void FrameUpdate()
    {
        if(Input::GetKeyDown('T'))
        {
            std::cout << "KeyDown" << '\n';
        }
        if (Input::GetKey('T'))
        {
            std::cout << "GetKey" << '\n';
        }
        if(Input::GetKeyUp('T'))
        {
            std::cout << "KeyUp" << '\n' << '\n';
        }

        //Frame Updates for GameObject/Component instances
        SendInitializeToEntities();
        SendFrameUpdateToEntities();
        SendDestroyToEntities();

        //Render
        entitySprites = entityData.GetEntitySprites();
        engineSystems.renderer->StartRenderCycle(entitySprites);
        entitySprites.clear();

        //Cleanup
        NCE::Input::FlushQueue();
        microSecondsSinceLastFrame = 0;

        //std::cout << "-End Frame-" << '\n';
    }

    void FixedUpdate()
    {
        //std::cout << '\n' << "-Start Fixed-" << '\n';
        // if (microSecondsSinceLastFixed / 1000000.0 > 0.051)
        // {
        //     std::cout << microSecondsSinceLastFixed / 1000000.0 << '\n';
        // }
        

        
        

        
        NCE::Containers::QuadTree colliderQuadTree(4, worldSpaceRect);

        entityColliders = entityData.GetEntityColliders();
        
        for (auto colliderPtr : entityColliders)
        {
            colliderQuadTree.AddElement(colliderPtr);
        }

        colliderQuadTree.CheckCollisions();

        //fix somehow?
        //entityColliders = entityData.GetEntityColliders();
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

        colliderQuadTree.Clear();
        entityColliders.clear();
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

    void RegisterEntity(Common::EntitySharedPtr t_entity)
    {
        if (t_entity == nullptr)
        {
            std::cout << "NCEMain.RegisterEntity() - t_entity is NULL" << '\n';
        }
        
        entityData.awaitingInitialize.insert({t_entity->GetID(), t_entity});
    }

    void DestroyEntity(int t_entityID)
    {
        int found = entityData.active.count(t_entityID);

        if (found)
        {
            auto entity = entityData.active.at(t_entityID);
            entityData.awaitingDestroy.insert( {t_entityID, entity} );
            entityData.active.erase(t_entityID);
        }
    }

}