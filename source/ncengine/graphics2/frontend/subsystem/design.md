
# Diligent DrawIndexedIndirectAttribs
```cpp
struct DrawIndexedIndirectAttribs
{
    /// The type of the elements in the index buffer.
    /// Allowed values: VT_UINT16 and VT_UINT32.
    VALUE_TYPE IndexType            DEFAULT_INITIALIZER(VT_UNDEFINED);

    /// A pointer to the buffer, from which indirect draw attributes will be read.
    ///
    /// The buffer must contain the following arguments at the specified offset:
    ///     Uint32 NumIndices;
    ///     Uint32 NumInstances;
    ///     Uint32 FirstIndexLocation;
    ///     Uint32 BaseVertex;
    ///     Uint32 FirstInstanceLocation
    IBuffer*  pAttribsBuffer        DEFAULT_INITIALIZER(nullptr);

    /// Offset from the beginning of the buffer to the location of the draw command attributes.
    Uint64 DrawArgsOffset           DEFAULT_INITIALIZER(0);

    /// Additional flags, see Diligent::DRAW_FLAGS.
    DRAW_FLAGS Flags                DEFAULT_INITIALIZER(DRAW_FLAG_NONE);

    /// The number of draw commands to execute. When the pCounterBuffer is not null, this member
    /// defines the maximum number of commands that will be executed.
    /// Must be less than DrawCommandProperties::MaxDrawIndirectCount.
    Uint32 DrawCount                DEFAULT_INITIALIZER(1);

    /// When DrawCount > 1, the byte stride between successive sets of draw parameters.
    /// Must be a multiple of 4 and greater than or equal to 20 bytes (sizeof(Uint32) * 5).
    Uint32 DrawArgsStride           DEFAULT_INITIALIZER(20);

    /// State transition mode for indirect draw arguments buffer.
    RESOURCE_STATE_TRANSITION_MODE AttribsBufferStateTransitionMode DEFAULT_INITIALIZER(RESOURCE_STATE_TRANSITION_MODE_NONE);


    /// A pointer to the optional buffer, from which Uint32 value with the draw count will be read.
    IBuffer* pCounterBuffer         DEFAULT_INITIALIZER(nullptr);

    /// When pCounterBuffer is not null, offset from the beginning of the counter buffer to the
    /// location of the command counter.
    Uint64 CounterOffset            DEFAULT_INITIALIZER(0);

    /// When counter buffer is not null, state transition mode for the count buffer.
    RESOURCE_STATE_TRANSITION_MODE CounterBufferStateTransitionMode DEFAULT_INITIALIZER(RESOURCE_STATE_TRANSITION_MODE_NONE);
}
```

# Shader Types
```cpp
struct TransformData
{
    DirectX::XMMATRIX matrix;
};

StructuredBuffer<TransformData>
HostBufferCache<TransformData>

struct MaterialData
{
    // simplified...
    uint32_t texture;
};

StructuredBuffer<MaterialData>
HostBufferCache<MaterialData>

// I don't think this buffer can behave like the others
//   - prob can't have holes
//   - prob needs to be sorted by mesh + passes
//   - needs duplicates then?
//   - need to know which ranges map to which passes + mesh combo
// Maybe an alternative here is to have a buffer per pass?
struct InstanceData
{
    uint32_t object;
    uint32_t material;
};

StructuredBuffer<InstanceData>
HostBufferCache<InstanceData>

struct DrawAttribs
{
    uint32_t numIndices;
    uint32_t numInstances;
    uint32_t firstIndexLocation;
    uint32_t baseVertex;
    uint32_t firstInstanceLocation;
};

// not sure, eventually writeable by compute shader
StructuredBuffer<DrawAttribs>
HostBufferCache<DrawAttribs>
```

# MeshRendererBatch
```cpp
struct MeshRendererBatch
{
    uint64_t meshId; // don't know if this needs to be here
    uint32_t instanceOffset;
    uint32_t instanceCount;
};

struct PassTargets
{
    sparse_map<MeshRendererBatch> batches; // maps meshId -> batch
    // need to track entities somehow. maybe?
    MaterialPass::type id;
};

class MaterialPassCache
{
    public:
        void AddTarget(MaterialPasses passes, uint64_t meshId) {
            ForEnabledPass(passes, [&](auto& pass) {
                ++pass.batches.at(meshId).instanceCount;
            });
        }

        void RemoveTarget(MaterialPasses passes, uint64_t meshId) {
            ForEnabledPasses(passes, [&](auto& pass) {
                --pass.batches.at(meshId).instanceCount;
            })
        }

        // need a way to set/update batch instance offset
        void AddBatch(uint64_t meshId, uint32_t instanceOffset) {
            ForAllPasses([&](auto& pass) {
                batches.emplace(meshId, MeshRendererBatch{meshId, instanceOffset, 0});
            });
        }

        void RemoveBatch(uint64_t meshId) {
            ForAllPasses([&](auto& pass) {
                batches.erase(meshId);
            });
        }

        void UpdateBatch(uint64_t meshId, uint32_t instanceOffset) {
            ForAllPasses([&](auto& pass) {
                batches.at(meshId).instanceOffset = instanceOffset;
            });
        }

    private:
        std::vector<PassTargets> m_passes;
};
```

# InstanceCache
```cpp
// class InstanceCache
// {
//     public:
//         auto AddInstance(Entity entity,
//                          uint32_t transformIndex,
//                          MaterialInstanceHandle materialIndex,
//                          MaterialPasses passes,
//                          uint64_t meshId) -> uint32_t
//         {
//             // todo: might not have passes/mesh yet
//             const auto baseOffset = m_ranges.at(passes).offset;
//             const auto meshOffset = m_ranges.at(passes).meshOffsets.at(meshId);
//             const auto instanceIndex = baseOffSet + meshOffset;
//             m_buffer.emplace(m_buffer.begin() + instanceindex, transformIndex, materialIndex);

//             for (const auto [passes, matRange] : std::views::zip(m_ranges.keys(), m_ranges.values()))
//             {
//                 if (meshRange <= baseOffset)
//             }

//             return instanceIndex;
//         }

//     private:
//         struct MaterialRange
//         {
//             sparse_map<uint32_t> meshOffsets; // maps meshId -> suboffset from material offset
//             uint32_t offset; // offset from begin where material range starts
//         };

//         std::vector<InstanceData> m_buffer;
//         sparse_map<MaterialBatch> m_ranges;
//         // [mat1 & mesh1], [mat1 & mesh1] [mat1 & mesh2], [mat2 & mesh2], [mat2 & mesh2]
// };


// - We sort instance buffer by meshId - when adding new instance, insert at end of current mesh location
// - New index is passed to each pass along with a bool indicating whether or not subsequent indices were modified
// - Passes store indices in order
// - When adding a new index, insert at correct location. If other indices modified, increment each index from
//   insertIndex + 1 to end
// - When building batches, use mechanics similar to CollectDirtyRange to identify per-pass batches:
//     - build 1 draw call per pass per 'run' of indices

struct InstanceAddResult
{
    uint32_t index;
    bool modified;
};

class InstanceCache
{
    struct Region
    {
        uint32_t offset;
        uint32_t count;
        uint32_t capacity;
    };

    public:
        auto AddInstance(uint64_t meshId,
                         uint32_t transformIndex,
                         MaterialInstanceHandle materialIndex) -> uint32_t
        {
            if (!m_regions.contains(meshId))
            {
                m_regions.emplace(m_buffer.size(), 0, 0);

                // or b/c at end (so we don't update other indices):
                // m_buffer.push_back({});
                // m_regions.emplace(size, 0, 1);
            }

            auto& region = m_regions.at(meshId);
            const auto index = region.offset + region.count;
            if (region.count == region.capacity)
            {
                ++region.count;
                ++region.capacity;
                m_buffer.insert(m_buffer.begin() + index, InstanceData{transformIndex, materialIndex});
                // update regions
                return {index, true};
            }
            else
            {
                ++region.count;
                m_buffer[index] = InstanceData{transformIndex, materialIndex};
                return {index, false};
            }
        }

        // want to compact Region, so could modify indices within Region
        void RemoveInstance(uint64_t meshId, uint32_t instanceIndex)
        {

        }

    private:
        std::vector<InstanceData> m_buffer;
        sparse_map<Region> m_regions; // maps meshId -> Region

        void UpdateSubsequentRegions(uint32_t regionOffset)
        {
            for (auto& region : m_regions)
            {
                if (region.offset > regionOffset)
                {
                    ++region.offset;
                }
            }
        }
};

struct PassBatch
{
    std::vector<uint32_t> instances;
    uint32_t indexCount;
    uint32_t firstIndex;
    uint32_t firstVertex;

    void Add(uint32_t instance, bool increment) {
        auto pos = get_pos(instances, instance);
        instances.sorted_insert(pos, instance);
        if (increment) {
            increment(pos, instances.end());
        }
    }

    auto Increment(uint32_t instance) {
        auto pos = get_pos(instances, instance);
        if (pos != instances.end()) {
            increment(pos, instances.end());
        }
    }
};

struct PassTargets
{
    sparse_map<PassBatch> batches;
    MaterialPass::type id;
};

class MaterialPassCache
{
    public:

        void AddTarget(MaterialPasses passes,
                       const asset::MeshView& mesh,
                       uint32_t instance,
                       bool subsequentIndicesModified)
        {
            ForEachPass(passes, [&](auto& pass) {
                if (pass.id & passes) {
                    if (!pass.batches.contains(mesh.id)) {
                        pass.batches.emplace(
                            mesh.id,
                            {},
                            mesh.indexCount,
                            mesh.firstIndex,
                            mesh.firstVertex
                        );
                    }

                    pass.batches.at(mesh.id).Add
                }
                else {

                }
            });
        }

    private:
        std::vector<PassTargets> m_passTargets;
};
```


# HostBufferCache
```cpp
template<...>
class HostBufferCache
{
    public:

    private:

};
```


# Operations

## MeshRenderer
### Add
```cpp
MeshRendererSubsystem::AddInstance(Entity entity,
                                   MaterialInstanceHandle material,
                                   MaterialPasses passes,
                                   const MeshView& mesh)
{
    const auto transformIndex = transformCache.Add(entity);
    const auto instance = instanceCache.Add(entity, transformIndex, material, passes, mesh.id);
    passCache.Add(passes, )
}
```

### Remove

### SetMesh

### SetMaterial

## Material
### Add

### Remove

### SetProperties

## Lights
todo

