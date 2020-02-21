#include "../include/TransformHierarchyData.h"

namespace nc
{
    TransformHierarchyData::TransformHierarchyData(int parentIndex)
    {
        m_hierarchy.push_back(parentIndex);
    }

    int TransformHierarchyData::GetParentIndex() const
    {
        return m_hierarchy[0];
    }

    std::vector<int> TransformHierarchyData::GetSiblingIndices() const
    {

    }

    std::vector<int> TransformHierarchyData::GetChildrenIndices() const
    {
        std::vector<int> childrenIndices;
        for(int x = 1; x < m_hierarchy.size(); ++x)
        {
            childrenIndices.push_back(m_hierarchy[x]);
        }

        return childrenIndices;
    }

    bool TransformHierarchyData::SetParentIndex(int parentIndex)
    {
        m_hierarchy[0] = parentIndex;
        return true;
    }

    bool TransformHierarchyData::AddChildIndex(int childIndex)
    {
        if(ContainsIndex(childIndex))
            return false;

        m_hierarchy.push_back(childIndex);
    }

    bool TransformHierarchyData::RemoveChildIndex(int childIndex)
    {
        int numberOfChildren = m_hierarchy.size();

        if(!ContainsIndex(childIndex) || numberOfChildren < 2)
            return false;

        int targetIndex;
        for(int x = 1; x < numberOfChildren; ++x)
        {
            if(m_hierarchy[x] == childIndex)
                targetIndex = x;
                break;
        }

        m_hierarchy[targetIndex] = m_hierarchy[numberOfChildren - 1];
        m_hierarchy.pop_back();
        return true;
    }

    bool TransformHierarchyData::ContainsIndex(int index) const
    {
        for(auto value : m_hierarchy)
        {
            if(value == index)
                return true;
        }
        return false;
    }


}