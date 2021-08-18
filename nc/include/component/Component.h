#pragma once
#include "Entity.h"

#include <concepts>
#include <type_traits>

namespace nc
{
    class AutoComponent;

    template<class T>
    concept Component = std::movable<T> &&
                        !std::same_as<Entity, T> &&
                        !std::derived_from<T, AutoComponent>;

    /** Base class for all Components. Only Components associated with a system
     *  should derive directly from ComponentBase. */
    class ComponentBase
    {
        public:
            explicit ComponentBase(Entity entity) noexcept
                : m_parentEntity{entity} {}

            ComponentBase(const ComponentBase&) = delete;
            ComponentBase(ComponentBase&&) = default;
            ComponentBase& operator=(const ComponentBase&) = delete;
            ComponentBase& operator=(ComponentBase&&) = default;

            Entity GetParentEntity() const noexcept { return m_parentEntity; }

        private:
            Entity m_parentEntity;
    };

    /** Base class for components with no associated system. */
    class AutoComponent : public ComponentBase
    {
        public:
            explicit AutoComponent(Entity entity) noexcept
                : ComponentBase{entity} {}

            virtual ~AutoComponent() = default;

            virtual void FrameUpdate(float) {}
            virtual void FixedUpdate() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(Entity) {}
            virtual void OnCollisionStay(Entity) {}
            virtual void OnCollisionExit(Entity) {}
            virtual void OnTriggerEnter(Entity) {}
            virtual void OnTriggerStay(Entity) {}
            virtual void OnTriggerExit(Entity) {}

            #ifdef NC_EDITOR_ENABLED
            virtual void ComponentGuiElement();
            #endif
    };

    /** Helper for configuring storage and allocation behavior. */
    template<class T>
    struct StoragePolicy
    {
        /** Allow destructor calls to be elided for types
         *  that don't satisfy std::is_trivially_destructible. */
        using allow_trivial_destruction = std::false_type;

        /** Dense views over sparse sets can be optionally sorted.
         *  This minimizes cache misses during iteration but results
         *  in slower additions and deletions. */
        using sort_dense_storage_by_address = std::true_type;

        /** Requires an OnAdd callback to be set in the registry. */
        using requires_on_add_callback = std::false_type;

        /** Requires an OnRemove callback to be set in the registry. */
        using requires_on_remove_callback = std::false_type;
    };

    /** Editor function that can be specialized to provide a custom widget.
     *  AutoComponents must use override their member function instead. */
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