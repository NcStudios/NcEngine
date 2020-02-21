#include "../include/TransformSystem.h"
#include <iostream>

namespace nc::internal{

/** @note should be fixed now that they're not pointers?: bug when vector is resized due to invalidating pointers */ 
TransformSystem::TransformSystem() { m_transforms.reserve(50); }

TransformSystem::~TransformSystem() {}

ComponentIndex TransformSystem::GetIndexFromHandle(const ComponentHandle handle)
{
    if (!Contains(handle)) return 0; //need value for unsuccessful
    return m_indexMap.at(handle);
}

void TransformSystem::MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
{
    if (!Contains(handle))
        m_indexMap.emplace(handle, targetIndex);
}

void TransformSystem::RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
{
    if (Contains(handle))
        m_indexMap.at(handle) = targetIndex;
}

const std::vector<Transform>& TransformSystem::GetVectorOfTransforms() const
{
    return m_transforms;
}

ComponentHandle TransformSystem::Add(const EntityHandle parentHandle)
{
    ComponentHandle handle = handleManager.GenerateNewHandle();
    m_transforms.push_back(Transform(handle, parentHandle));
    ComponentIndex lastIndex = m_transforms.size() - 1;
    MapHandleToIndex(handle, lastIndex);
    return handle;
}

bool TransformSystem::Remove(const ComponentHandle handle)
{
    if (!Contains(handle))
    {
        return false;
    }

    ComponentIndex toRemove = GetIndexFromHandle(handle);
    ComponentIndex lastIndex = m_transforms.size() - 1;
    ComponentHandle lastElementHandle = m_transforms.at(lastIndex).GetHandle(); //no good

    m_transforms.at(toRemove) = m_transforms.at(lastIndex);
    m_transforms.pop_back();
    if(m_indexMap.erase(handle) != 1)
        std::cout << "problem erasing\n";
    RemapHandleToIndex(lastElementHandle, toRemove);
    
    return true;
}

bool TransformSystem::Contains(const ComponentHandle handle) const
{
    return m_indexMap.count(handle) > 0;
}

Transform* TransformSystem::GetPointerTo(const ComponentHandle handle)
{
    if (!Contains(handle)) return nullptr;
    ComponentIndex index = GetIndexFromHandle(handle);
    return &m_transforms.at(index);
}

} //end namespace nc::internal