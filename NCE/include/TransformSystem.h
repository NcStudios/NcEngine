#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include <unordered_map>
#include "NCE.h"
#include "HandleManager.h"

namespace nc::internal
{
    class TransformSystem
    {
        private:
            std::vector<Transform> m_transforms;
            std::unordered_map<ComponentHandle, ComponentIndex> m_indexMap;
            HandleManager<ComponentHandle> handleManager;

            ComponentIndex GetIndexFromHandle(const ComponentHandle handle);
            void MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
            void RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);

        public:
            TransformSystem();
            virtual ~TransformSystem();

            const std::vector<Transform>& GetVectorOfTransforms() const;
            ComponentHandle virtual Add(const EntityHandle parentHandle);
            bool virtual Remove(const ComponentHandle handle);
            bool virtual Contains(const ComponentHandle handle) const;
            Transform* GetPointerTo(const ComponentHandle handle);
    };
} //end namespace nc::internal

#endif