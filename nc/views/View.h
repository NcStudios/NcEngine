#pragma once
#include "EntityView.h"

namespace nc 
{
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

    template<>
    inline PointLight* View<PointLight>(Component* component)
    {
        return component->GetParentView()->GetPointLight();
    }
}