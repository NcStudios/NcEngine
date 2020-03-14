#include "EntityView.h"
#include "NCE.h"

namespace nc
{
    EntityView::EntityView()
        : entityHandle(0), transformHandle(0)
    {}

    EntityView::EntityView(EntityHandle handle, ComponentHandle transHandle)
        : entityHandle(handle), transformHandle(transHandle) 
    {}

    EntityView::EntityView(const EntityView& other)
    {
        entityHandle = other.entityHandle;
        transformHandle = other.transformHandle;
    }

    EntityView::EntityView(EntityView&& other)
    {
        entityHandle = other.entityHandle;
        transformHandle = other.transformHandle;
    }

    EntityView& EntityView::operator=(const EntityView& other)
    {
        entityHandle = other.entityHandle;
        transformHandle = other.transformHandle;
        return *this;
    }

    EntityView& EntityView::operator=(EntityView&& other)
    {
        entityHandle = other.entityHandle;
        transformHandle = other.transformHandle;
        return *this;
    }

    EntityView* EntityView::MainCamera() noexcept
    {
        return NCE::GetMainCamera();
    }

    Entity* EntityView::Entity() noexcept
    {
        return NCE::GetEntity(entityHandle);
    }
    
    Transform* EntityView::Transform() noexcept
    {
        return NCE::GetTransform(transformHandle);
    }

    nc::Renderer* EntityView::Renderer() noexcept
    {
        return NCE::GetRenderer(entityHandle);
    }

    nc::Renderer* EntityView::AddRenderer() noexcept
    {
        return NCE::AddRenderer(entityHandle);
    }


    
} // end namespace nc