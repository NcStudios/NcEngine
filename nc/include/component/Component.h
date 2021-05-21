#pragma once
#include "entity/EntityHandle.h"

#include <type_traits>

namespace nc
{
    class Entity;

    /** Base class for all Components. Only Components associated with a system
     *  should derive directly from ComponentBase. */
    class ComponentBase
    {
        public:
            ComponentBase(EntityHandle handle) noexcept
                : m_parentHandle{handle} {}

            virtual ~ComponentBase() = default;
            ComponentBase(const ComponentBase&) = delete;
            ComponentBase(ComponentBase&&) = default;
            ComponentBase& operator=(const ComponentBase&) = delete;
            ComponentBase& operator=(ComponentBase&&) = default;

            EntityHandle GetParentHandle() const noexcept { return m_parentHandle; }

            #ifdef NC_EDITOR_ENABLED
            virtual void EditorGuiElement();
            #endif

        private:
            EntityHandle m_parentHandle;
    };

    /** Base class for user-defined Components. */
    class Component : public ComponentBase
    {
        public:
            Component(EntityHandle handle) noexcept
                : ComponentBase{handle} {}

            virtual void FrameUpdate(float) {}
            virtual void FixedUpdate() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(Entity*) {}
            virtual void OnCollisionStay(Entity*) {};
            virtual void OnCollisionExit(Entity*) {};
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
    };
} //end namespace nc