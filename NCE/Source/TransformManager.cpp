#include "../Include/TransformManager.h"


/** @note There is a bug when vector is resized due to invalidating pointers */ 
TransformManager::TransformManager() { m_transforms.reserve(50); }

TransformManager::~TransformManager() {}

ComponentIndex TransformManager::GetIndexFromHandle(const ComponentHandle handle)
{
    if (!Contains(handle)) return 0; //need value for unsuccessful
    return m_indexMap.at(handle);
}

void TransformManager::MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
{
    if (!Contains(handle))
        m_indexMap.emplace(handle, targetIndex);
}

void TransformManager::RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex)
{
    if (Contains(handle))
        m_indexMap.at(handle) = targetIndex;
}

const std::vector<Transform>& TransformManager::GetVectorOfTransforms() const
{
    return m_transforms;
}

ComponentHandle TransformManager::Add(const EntityHandle parentHandle)
{
    ComponentHandle handle = handleManager.GenerateNewHandle();
    m_transforms.push_back(Transform(handle, parentHandle));
    ComponentIndex lastIndex = m_transforms.size() - 1;
    MapHandleToIndex(handle, lastIndex);
    return handle;
}

bool TransformManager::Remove(const ComponentHandle handle)
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

bool TransformManager::Contains(const ComponentHandle handle)
{
    return m_indexMap.count(handle) > 0;
}

Transform* TransformManager::GetPointerTo(const ComponentHandle handle)
{
    if (!Contains(handle)) return nullptr;
    ComponentIndex index = GetIndexFromHandle(handle);
    return &m_transforms.at(index);
}
