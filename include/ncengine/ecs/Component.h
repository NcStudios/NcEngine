/**
 * @file Component.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Entity.h"
#include "ncengine/type/Type.h"
#include "ncengine/utility/Signal.h"

#include <any>
#include <concepts>
#include <iosfwd>
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
    /** @brief Enable the OnAdd Signal in the component's pool. */
    static constexpr bool EnableOnAddCallbacks = false;

    /** @brief Enable the OnCommit Signal in the component's pool. */
    static constexpr bool EnableOnCommitCallbacks = false;

    /** @brief Enable the OnRemove Signal in the component's pool. */
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

struct SerializationContext;
struct DeserializationContext;

namespace ui::editor
{
struct EditorContext;
} // namespace ui::editor

/** @brief Optional data and callbacks for generic component operations. */
template<PooledComponent T>
struct ComponentHandler
{
    /** @brief Function type for the Factory handler. */
    using Factory_t = std::function<T(Entity entity, const std::any& userData)>;

    /** @brief Function type for the serialize handler. */
    using Serialize_t = std::function<void(std::ostream& binaryStream,
                                           const T& component,
                                           const SerializationContext& ctx,
                                           const std::any& userData)>;

    /** @brief Function type for the deserialize handler. */
    using Deserialize_t = std::function<T(std::istream& binaryStream,
                                          const DeserializationContext& ctx,
                                          const std::any& userData)>;

    /** @brief Function type for the DrawUI handler. */
    using DrawUI_t = std::function<void(T& component,
                                        Entity self,
                                        ui::editor::EditorContext& ctx)>;

    /**
     * @brief A unique identifier for the component.
     * @note Set to 0 to get assigned an available id. The range [1, 100] is reserved for engine
     *       components. Automatically assigned ids are assigned in reverse order starting from
     *       std::numeric_limits<size_t>::max().
    */
    size_t id = 0ull;

    /** @brief A name for the component with no uniqueness constraints. */
    std::string name = "User Component";

    /** @brief Optional user data, which is passed to the factory, serialize, and deserialize handlers. */
    std::any userData = std::any{};

    /** @brief Callback for creating an instance of T.
     *  @note This enables adding/removing types through the editor. */
    Factory_t factory = nullptr;

    /** @brief Callback for serializing an instance of T.
     *  @note This is called when serializing scene fragments if and only if both serialize and deserialize are set. */
    Serialize_t serialize = nullptr;

    /** @brief Callback for deserializing an instance of T.
     *  @note This is called when deserializing scene fragments if and only if both serialize and deserialize are set. */
    Deserialize_t deserialize = nullptr;

    /** @brief Callback for drawing T's UI widget. */
    DrawUI_t drawUI = nullptr;
};
} // namespace nc
