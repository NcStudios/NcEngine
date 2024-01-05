/**
 * @file Component.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Entity.h"
#include "detail/EditorMacros.h"
#include "ncengine/type/Type.h"
#include "ncengine/utility/Signal.h"

#include <concepts>
#include <type_traits>

namespace nc
{
/**
 * @brief Optional base class for components.
 * 
 * This is the conventional interface used by engine components for providing
 * access to their parent `Entity`. Note that `Registry::GetParent()` can find
 * this value for any component, so types only need to derive from this if they
 * want faster `Entity` retrieval.
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

/** @brief Requirements for the Registry to recognize a pooled component. */
template<class T>
concept PooledComponent = std::movable<std::remove_const_t<T>> &&
                          !std::derived_from<T, FreeComponent> &&
                          !std::same_as<T, Entity>;

template<class T>
concept Component = PooledComponent<T> || std::derived_from<T, FreeComponent>;

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

/** @brief Optional data and callbacks for generic component operations. */
template<PooledComponent T>
struct ComponentHandler
{
    /** @brief Function type for the Factory handler. */
    using Factory_t = std::function<T(Entity, void*)>;

    /** @brief Function type for the DrawUI handler. */
    using DrawUI_t = std::function<void(T&)>;

    /**
     * @brief A unique identifier for the component.
     * @note Set to 0 to get assigned an available id. The range [0, 100]
     *       is reserved for engine components. Automatically assigned ids are assigned in reverse order
     *       starting from std::numeric_limits<size_t>::max().
    */
    size_t id = 0ull;

    /** @brief A name for the component with no uniqueness constraints. */
    std::string name = "User Component";

    /** @brief Optional pointer to user data, which is passed to the factory handler. */
    void* userData = nullptr;

    /** @brief Handler for creating an instance of T.
     *  @note Factory handlers enable adding/removing types through the editor. */
    Factory_t factory = nullptr;

    /** @brief Handler for drawing T's UI widget. */
    DrawUI_t drawUI = nullptr;
};
} // namespace nc
