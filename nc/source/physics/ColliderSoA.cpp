#include "ColliderSoA.h"

namespace nc::physics
{
    ColliderSoA::ColliderSoA(size_t maxColliders)
        : handles{},
          transforms{maxColliders},
          volumeData{maxColliders},
          types{maxColliders},
          gaps{},
          nextFree{0u}
    {
        /** @todo fix this */
        handles.resize(maxColliders);
    }

    IndexData::IndexData()
        : handle{EntityHandle::Invalid()},
          index{0u},
          container{nullptr}
    {
    }

    IndexData::IndexData(EntityHandle handle_, uint32_t index_, ColliderSoA* container_)
        : handle{handle_},
          index{index_},
          container{container_}
    {
    }
} // namespace nc::physics