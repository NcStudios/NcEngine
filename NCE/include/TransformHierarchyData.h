#ifndef TRANSFORM_HIERARCHY_DATA_H
#define TRANSFORM_HIERARCHY_DATA_H

#include <vector>

namespace nc
{
    const int ROOT_TRANSFORM = -1;

    class TransformHierarchyData
    {
        public:
            TransformHierarchyData(int parentIndex = ROOT_TRANSFORM);

            int GetParentIndex() const;
            std::vector<int> GetSiblingIndices() const;
            std::vector<int> GetChildrenIndices() const;

            bool SetParentIndex(int parentIndex);
            bool AddChildIndex(int childIndex);
            bool RemoveChildIndex(int childIndex);

        private:
            std::vector<int> m_hierarchy;

            bool ContainsIndex(int index) const;
    };
}// end namespace nc

#endif