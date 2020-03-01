// #pragma once
// #include <unordered_map>

// #include "Transform.h" //can forward declare?

// namespace nc::internal
// {
//     class TransformSystem :
//     {
//         public:
//             TransformSystem();
//             virtual ~TransformSystem();

        
//              ComponentHandle Add(const EntityHandle parentHandle) override;
//              bool Remove(const ComponentHandle handle) override;
//              bool virtual Contains(const ComponentHandle handle) const override;
//              const std::vector<Transform>& GetVector() const override;
//              Transform* GetPointerTo(const ComponentHandle handle) const override;

//         private:
//             std::vector<Transform> m_transforms;
//             std::unordered_map<ComponentHandle, ComponentIndex> m_indexMap;
//             HandleManager<ComponentHandle> handleManager;

//             ComponentIndex GetIndexFromHandle(const ComponentHandle handle);
//             void MapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
//             void RemapHandleToIndex(const ComponentHandle handle, const ComponentIndex targetIndex);
//     };

// } //end namespace nc::internal
