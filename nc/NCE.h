#pragma once
#include <string>
#include "Common.h"
#include "Entity.h"
#include "component/Component.h"
#include "component/Transform.h"
#include "graphics/Mesh.h"
#include "input/Input.h"
#include "math/NCVector.h"
#include "scene/Scene.h"
#include "time/NCTime.h"
#include "debug/NcException.h"

namespace nc
{
    namespace engine { class Engine; }
    namespace graphics { class Graphics; }
    class Renderer;
    class Camera;
    class PointLight;

    class NCE
    {
        public:
            NCE(engine::Engine* enginePtr);

            /**
             * Clean up and shut down engine.
             */
            static void Exit();

            static void ChangeScene(std::unique_ptr<scene::Scene>&& scene);

            /**
             * Sets the camera that NCE::GetMainCameraTransform()
             * refers to.
             */
            static void RegisterMainCamera(Camera * camera);

            /** 
             * Get pointer to the main camera.
             * @return Pointer to main camera's transform, nullptr 
             * if no camera has been registered.
             */
            static Transform * GetMainCameraTransform();

            /***************
             * Entity Access
             * 
             ***************/
            /**
             * Create default constructed Entity.
             * @return EntityHandle for the new Entity.
             */
            static EntityHandle CreateEntity();

            /**
             * Create Entity with specified Transform data and tag.
             * @param pos Initial position.
             * @param rot Initial rotation.
             * @param scale Initial scale.
             * @param tag Name for entity (no validation for unique tags).
             * @return EntityHandle for the new Entity.
             */
            static EntityHandle CreateEntity(const Vector3& pos, const Vector3& rot, const Vector3& scale, const std::string& tag);

            /**
             * Get ptr to an Entity
             * @return Non-owning, non-cacheable ptr on success,
             *         nullptr on failure.
             */
            static Entity * GetEntity(EntityHandle handle);

            /**
             * Get first active Entity matching tag.
             * @return Non-owning, non-cacheable ptr on success,
             *         nullptr on failure.
             */
            static Entity* GetEntity(const std::string& tag); //finds first active entity with specified tag, returns nullptr if not found

            /**
             * Mark Entity for destruction.
             * @note OnDestroy() will be called at end of current frame.
             */
            static bool DestroyEntity(EntityHandle handle);

            /****************************
             * User Component Access
             * (user defined class 
             *  extending nc::Component)
             ***************************/
            /**
             * Check if Entity has a user component.
             * @param handle Handle of the Entity.
             * @return True if found, false on failure.
             * @throw BadHandle
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static bool HasUserComponent(const EntityHandle handle) noexcept(false);

            /**
             * Add new user component to Entity.
             * @param handle Handle of the Entity.
             * @return Non-owning, non-cacheable pointer to T on success.
             * @throw BadHandle
             * @note Returned ptr not safe to cache.
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type,
                     class ... Args>
            static T * AddUserComponent(const EntityHandle handle, Args&& ... args) noexcept(false);

            /**
             * Remove user component from Entity.
             * @param handle Handle of the Entity.
             * @return True if removed, false on failure.
             * @throw BadHandle
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static bool RemoveUserComponent(const EntityHandle handle) noexcept(false);

            /**
             * Gets user component from Entity.
             * @param handle Handle of the Entity.
             * @return Non-owning, non-cacheable pointer to T on success,
             *         nullptr on failure.
             * @throw BadHandle
             * @note Returned ptr not safe to cache.
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static T * GetUserComponent(const EntityHandle handle) noexcept(false); 

            /****************************
             * Engine Component Access
             * (Transform, Renderer, ...)
            *****************************/
           /**
             * Get a Component's Transform.
             * @param handle The transform's handle.
             * @return Pointer to Transform on success, nullptr on failure.
             * @note Returned ptr not safe to cache.
             */
            static Transform* GetTransformFromHandle(const ComponentHandle handle);

            /**
             * Get and Entity's Transform.
             * @param handle EntityHandle of the transform's parent.
             * @return Pointer to Transform on success, nullptr on failure.
             * @note Returned ptr not safe to cache.
             */
            static Transform* GetTransformFromEntityHandle(const EntityHandle handle);

            /**
             * Check if Entity has an engine component.
             * @param handle Handle of the Entity.
             * @return True if found, false on failure.
             * @throw BadHandle
             */           
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static bool HasEngineComponent(const EntityHandle handle) noexcept(false);

            /**
             * Adds new engine component to Entity.
             * @param handle Handle of the Entity.
             * @return Non-owning, non-cacheable pointer to T on success.
             * @throw BadHandle
             * @note Returned ptr not safe to cache.
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static T * AddEngineComponent(const EntityHandle handle) noexcept(false);

            /**
             * Adds new engine component to Entity.
             * @param handle Handle of the Entity.
             * @return Non-owning, non-cacheable pointer to T on success.
             * @throw BadHandle
             * @note Returned ptr not safe to cache.
             */
            template<class Renderer_t,
                     //class = typename std::enable_if<std::is_same<Renderer, Renderer_t>::value>::type>
                     class = typename std::enable_if<std::is_base_of<Component, Renderer_t>::value>::type>
            static Renderer_t * AddEngineComponent(const EntityHandle handle, graphics::Mesh& mesh) noexcept(false);

            /**
             * Remove engine component from Entity.
             * @param handle Handle of the Entity.
             * @return True if removed, false on failure.
             * @throw BadHandle
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static bool RemoveEngineComponent(const EntityHandle handle) noexcept(false);

            /**
             * Gets engine component from Entity.
             * @param handle Handle of the Entity.
             * @return Non-owning, non-cacheable pointer to T on success,
             *         nullptr on failure.
             * @throw BadHandle
             * @note Returned ptr not safe to cache.
             */
            template<class T,
                     class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
            static T * GetEngineComponent(const EntityHandle handle) noexcept(false);


        private:
            static engine::Engine* m_engine;

    }; //end class NCE
    

    /**************************
     * Template Definitions
     * (specializations in cpp)
     **************************/
    template<class T, class>
    bool NCE::HasUserComponent(const EntityHandle handle) noexcept(false)
    {
        auto hnd = NCE::GetEntity(handle);
        IF_THROW(hnd == nullptr, "NCE::HasUserComponent : bad handle");
        return hnd->HasUserComponent<T>();
    }

    template<class T, class, class ... Args>
    T * NCE::AddUserComponent(const EntityHandle handle, Args&& ... args) noexcept(false)
    {
        auto ptr = NCE::GetEntity(handle);
        IF_THROW(ptr == nullptr, "NCE::AddUserComponent : bad handle");
        return ptr->AddUserComponent<T>(std::forward(args)...);
    }

    template<class T, class>
    bool NCE::RemoveUserComponent(const EntityHandle handle) noexcept(false)
    {
        auto hnd = NCE::GetEntity(handle);
        IF_THROW(hnd == nullptr, "NCE::RemoveUserComponent : bad handle");
        return hnd->RemoveUserComponent<T>();
    }

    template<class T, class>
    T * NCE::GetUserComponent(const EntityHandle handle) noexcept(false)
    {
        auto hnd = NCE::GetEntity(handle);
        IF_THROW(hnd == nullptr, "NCE::GetUserComponent : bad handle");
        return hnd->GetUserComponent<T>();
    }
} //end namespace nc