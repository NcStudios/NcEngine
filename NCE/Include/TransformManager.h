#ifndef TRANSFORM_MANAGER
#define TRANSFORM_MANAGER


#include "Common.h"
#include "NCE.h"
#include "HandleManager.h"
#include "Transform.h"

class TransformManager
{
    private:
        std::vector<Transform> m_transforms;
        std::unordered_map<ComponentHandle, ComponentIndex> m_indexMap;
        HandleManager<ComponentHandle> handleManager;

        ComponentIndex GetIndexFromHandle(const ComponentHandle handle);
        void MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
        void RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);

    public:
        TransformManager();
        virtual ~TransformManager();

        const std::vector<Transform>& GetVectorOfTransforms() const;
        ComponentHandle virtual Add(const EntityHandle parentHandle);
        bool virtual Remove(const ComponentHandle handle);
        bool virtual Contains(const ComponentHandle handle);
        Transform* GetPointerTo(const ComponentHandle handle);
};

#endif