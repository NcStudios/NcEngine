#pragma once
#include "Common.h"
#include "EntityView.h"


#include <type_traits>

namespace nc
{
    class Component
    {
        public:
            Component(ComponentHandle handle, EntityView parentView) noexcept : m_handle(handle), m_parentView(parentView) {}
            Component(Component&& other) : m_handle(other.m_handle), m_parentView(other.m_parentView) {}
            Component& operator=(Component&& other)
            {
                m_handle = other.m_handle; 
                other.m_handle = 0u;
                m_parentView = other.m_parentView;
                return *this;
            }

            virtual ~Component() {}

            ComponentHandle GetHandle() const noexcept { return m_handle; }
            EntityView* GetParentView() noexcept { return &m_parentView; }
            //EntityView View() const noexcept { return m_parentView; }

            virtual void FrameUpdate() {}
            virtual void FixedUpdate() {}
            virtual void OnInitialize() {}
            virtual void OnDestroy() {}
            virtual void OnCollisionEnter(const EntityHandle other) {}
            virtual void OnCollisionStay() {}
            virtual void OnCollisionExit() {}

        private:
            ComponentHandle m_handle;
            EntityView m_parentView;
    };


    //maybe move specializations to cpp to avoid relying on inline?
    
    //View Target type as ViewAs type. [unsafe to cache]
    template<typename Target, typename ViewAs = Target>
    ViewAs* View(Component*);

    //View into component's EntityVie [safe to cache]
    template<>
    inline EntityView* View<EntityView>(Component* component)
    {
        return component->GetParentView();
    }

    //View into component's Entity [unsafe to cache]
    template<>
    inline Entity* View<Entity>(Component* component)
    {
        return component->GetParentView()->Entity();
    }

    //View into component's Transform [unsafe to cache]
    template<>
    inline Transform* View<Transform>(Component* component)
    {
        return component->GetParentView()->Transform();
    }

    //View into component's Renderer [unsafe to cache]
    template<>
    inline Renderer* View<Renderer>(Component* component)
    {
        return component->GetParentView()->Renderer();
    }

    //View into the main camera's entity view [safe to cache]
    template<>
    inline EntityView* View<Camera, EntityView>(Component* component)
    {
        return component->GetParentView()->MainCamera();
    }

    //View into the main camera's transform [unsafe to cache]
    template<>
    inline Transform* View<Camera, Transform>(Component* component)
    {
        return component->GetParentView()->MainCamera()->Transform();
    }


} //end namespace nc