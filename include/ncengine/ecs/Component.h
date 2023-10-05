#pragma once

#include "Entity.h"
#include "detail/EditorMacros.h"
#include "ncengine/utility/Signal.h"

#include <concepts>
#include <type_traits>

namespace nc
{
/**
 * @brief Base class for all components.
 * 
 * User-defined pooled components should derive directly from this. These types
 * must be registered with Registry::RegisterComponentType<T>() before use.
 */
class ComponentBase
{
    public:
        explicit ComponentBase(Entity entity) noexcept
            : m_parentEntity{entity} {}

        ComponentBase(const ComponentBase&) = delete;
        ComponentBase(ComponentBase&&) = default;
        ComponentBase& operator=(const ComponentBase&) = delete;
        ComponentBase& operator=(ComponentBase&&) = default;

        Entity ParentEntity() const noexcept { return m_parentEntity; }

    private:
        Entity m_parentEntity;
};

/**
 * @brief Base class for free components.
 * 
 * User-defined free components should derive from this to avoid creating
 * distinct pools for each type. They do not need to be registered.
 * 
 */
class FreeComponent : public ComponentBase
{
    public:
        explicit FreeComponent(Entity entity) noexcept
            : ComponentBase{entity} {}

        virtual ~FreeComponent() = default;

        #ifdef NC_EDITOR_ENABLED
        virtual void ComponentGuiElement();
        #endif
};

/** @brief Type representation of a null component. */
struct NullComponent : public ComponentBase
{
    NullComponent(Entity entity) : ComponentBase{entity} {}
};

/** @brief Requirements for the Registry to recognize a pooled component. */
template<class T>
concept PooledComponent = std::movable<std::remove_const_t<T>> &&
                          std::derived_from<T, ComponentBase> &&
                          !std::derived_from<T, FreeComponent>;

/** @brief Default storage behavior for pooled components. */
struct DefaultStoragePolicy
{
    /** @brief Allows OnAdd callbacks to be set in the registry. */
    static constexpr bool EnableOnAddCallbacks = false;

    /** @brief Allows OnRemove callbacks to be set in the registry. */
    static constexpr bool EnableOnRemoveCallbacks = false;
};

/**
 * @brief Provide a specialization to customize storage options and
 * behavior for a user-defined type.
 * 
 * @tparam T A component, which models PooledComponent, to customize.
 */
template<PooledComponent T>
struct StoragePolicy : DefaultStoragePolicy {};

/** @brief Optional callbacks for generic component operations. */
template<PooledComponent T>
struct ComponentHandler
{
    /** @brief Function type for the DrawUI handler. */
    using DrawUI_t = std::function<void(T&)>;

    /** @brief Signal type for on add events. */
    using OnAdd_t = Signal<T&>;

    /** @brief Signal type for on remove events.*/
    using OnRemove_t = Signal<Entity>;

    /** @brief Handler for drawing T's UI widget. */
    DrawUI_t drawUI = [](T&){};

    /** @brief Signal emitted when an instance of T is created.
     *  @note Requires StoragePolicy<T>::EnableOnAddCallbacks. */
    OnAdd_t onAdd = {};

    /** @brief Signal emitted when an instance of T is destroyed.
     *  @note Requires StoragePolicy<T>::EnableOnRemoveCallbacks. */
    OnRemove_t onRemove = {};
};

#ifdef NC_EDITOR_ENABLED
namespace internal
{
    void DefaultComponentGuiElement();
}

/** @brief Provide a specialization to customize a pooled component's editor widget. */
template<class T>
void ComponentGuiElement(T*)
{
    internal::DefaultComponentGuiElement();
}
#endif
} // namespace nc
