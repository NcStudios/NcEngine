#pragma once
#include "EditorMeta.h"
#include "ecs/Entity.h"

#include <concepts>
#include <type_traits>

namespace nc
{
    /** Base class for all Components. Pooled components should derive
     *  directly from this. */
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

    /** Base class for free components (unpooled). */
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

    /** Requirements for the Registry to recognize a pooled component. */
    template<class T>
    concept PooledComponent = std::movable<T> &&
                              std::derived_from<T, ComponentBase> &&
                              !std::derived_from<T, FreeComponent>;

    /** Helper for configuring storage and allocation behavior. */
    template<class T>
    struct StoragePolicy
    {
        /** Requires an OnAdd callback to be set in the registry. */
        using requires_on_add_callback = std::false_type;

        /** Requires an OnRemove callback to be set in the registry. */
        using requires_on_remove_callback = std::false_type;
    };

    /** Editor function that can be specialized to provide a custom widget.
     *  FreeComponents must use override their member function instead. */
    #ifdef NC_EDITOR_ENABLED
    namespace internal
    {
        void DefaultComponentGuiElement();
    }

    template<class T>
    void ComponentGuiElement(T*)
    {
        internal::DefaultComponentGuiElement();
    }
    #endif
} //end namespace nc