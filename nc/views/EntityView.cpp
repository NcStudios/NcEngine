#include "EntityView.h"
#include "NCE.h"

namespace nc
{
    EntityView::EntityView()
        : Handle(0), TransformHandle(0)
    {}

    EntityView::EntityView(EntityHandle handle, ComponentHandle transHandle)
        : Handle(handle), TransformHandle(transHandle) 
    {}

    EntityView::EntityView(const EntityView& other)
    {
        Handle = other.Handle;
        TransformHandle = other.TransformHandle;
    }

    EntityView::EntityView(EntityView&& other)
    {
        Handle = other.Handle;
        TransformHandle = other.TransformHandle;
    }

    EntityView& EntityView::operator=(const EntityView& other)
    {
        Handle = other.Handle;
        TransformHandle = other.TransformHandle;
        return *this;
    }

    EntityView& EntityView::operator=(EntityView&& other)
    {
        Handle = other.Handle;
        TransformHandle = other.TransformHandle;
        return *this;
    }
    
    bool EntityView::operator==(const EntityView& other) const
    {
        return (Handle == other.Handle) && (TransformHandle == other.TransformHandle);
    }

    bool EntityView::operator!=(const EntityView& other) const
    {
        return !(*this == other);
    }
} // end namespace nc