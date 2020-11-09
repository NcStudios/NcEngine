#pragma once
#include "ecs/Entity.h"
#include "component/Transform.h"
#include "NcDebug.h"

namespace nc
{
    class Renderer;
    class PointLight;
    namespace graphics { class Graphics; class Mesh; class PBRMaterial; }
    namespace math { class Vector3; }

    /**
     * Create default constructed Entity.
     * @return EntityHandle for the new Entity.
     */
    EntityHandle NcCreateEntity();

    /**
     * Create Entity with specified Transform data and tag.
     * @param pos Initial position.
     * @param rot Initial rotation.
     * @param scale Initial scale.
     * @param tag Name for entity (no validation for unique tags).
     * @return EntityHandle for the new Entity.
     */
    EntityHandle NcCreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag);

    /**
     * Get ptr to an Entity
     * @return Non-owning ptr on success, nullptr on failure.
     */
    Entity * NcGetEntity(EntityHandle handle);

    /**
     * Get first active Entity matching tag.
     * @return Non-owning ptr on success, nullptr on failure.
     */
    Entity* NcGetEntity(std::string tag); //finds first active entity with specified tag, returns nullptr if not found

    /**
     * Mark Entity for destruction.
     * @note OnDestroy() will be called at end of current frame.
     */
    bool NcDestroyEntity(EntityHandle handle);

    /**
     * Check if Entity has a user component.
     * @param handle Handle of the Entity.
     * @return True if found, false on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    bool NcHasUserComponent(const EntityHandle handle) noexcept(false);

    /**
     * Add new user component to Entity.
     * @param handle Handle of the Entity.
     * @return Non-owning pointer to T on success, nullptr on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type,
                class ... Args>
    T * NcAddUserComponent(const EntityHandle handle, Args&& ... args) noexcept(false);

    /**
     * Remove user component from Entity.
     * @param handle Handle of the Entity.
     * @return True if removed, false on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    bool NcRemoveUserComponent(const EntityHandle handle) noexcept(false);

    /**
     * Gets user component from Entity.
     * @param handle Handle of the Entity.
     * @return Non-owning pointer to T on success, nullptr on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    T * NcGetUserComponent(const EntityHandle handle) noexcept(false); 

    /**
     * Get a Component's Transform.
     * @param handle The transform's handle.
     * @return Pointer to Transform on success, nullptr on failure.
     */
    Transform* NcGetTransform(const ComponentHandle handle) noexcept(false);

    /**
     * Check if Entity has an engine component.
     * @param handle Handle of the Entity.
     * @return True if found, false on failure.
     * @throw BadHandle
     */           
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    bool NcHasEngineComponent(const EntityHandle handle) noexcept(false);

    /**
     * Adds new engine component to Entity.
     * @param handle Handle of the Entity.
     * @return Non-owning pointer to T on success.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    T * NcAddEngineComponent(const EntityHandle handle) noexcept(false);

    /**
     * Adds new engine component to Entity.
     * @param handle Handle of the Entity.
     * @return Non-owning pointer to T on success.
     * @throw BadHandle
     */
    template<class Renderer_t,
                class = typename std::enable_if<std::is_base_of<Component, Renderer_t>::value>::type>
    Renderer_t * NcAddEngineComponent(const EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material) noexcept(false);

    /**
     * Remove engine component from Entity.
     * @param handle Handle of the Entity.
     * @return True if removed, false on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    bool NcRemoveEngineComponent(const EntityHandle handle) noexcept(false);

    /**
     * Gets engine component from Entity.
     * @param handle Handle of the Entity.
     * @return Non-owning pointer to T on success, nullptr on failure.
     * @throw BadHandle
     */
    template<class T,
                class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
    T * NcGetEngineComponent(const EntityHandle handle) noexcept(false);

} // end namespace nc

template<class T, class>
bool nc::NcHasUserComponent(const EntityHandle handle) noexcept(false)
{
    auto ptr = nc::NcGetEntity(handle);
    IF_THROW(ptr == nullptr, "NcHasUserComponent : bad handle");
    return ptr->HasUserComponent<T>();
}

template<class T, class, class ... Args>
T * nc::NcAddUserComponent(const EntityHandle handle, Args&& ... args) noexcept(false)
{
    auto ptr = nc::NcGetEntity(handle);
    IF_THROW(ptr == nullptr, "NcAddUserComponent : bad handle");
    return ptr->AddUserComponent<T>(std::forward<Args>(args)...);
}

template<class T, class>
bool nc::NcRemoveUserComponent(const EntityHandle handle) noexcept(false)
{
    auto ptr = nc::NcGetEntity(handle);
    IF_THROW(ptr == nullptr, "NcRemoveUserComponent : bad handle");
    return ptr->RemoveUserComponent<T>();
}

template<class T, class>
T * nc::NcGetUserComponent(const EntityHandle handle) noexcept(false)
{
    auto ptr = nc::NcGetEntity(handle);
    IF_THROW(ptr == nullptr, "NcGetUserComponent : bad handle");
    return ptr->GetUserComponent<T>();
}