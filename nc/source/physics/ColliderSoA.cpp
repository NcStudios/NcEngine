#include "ColliderSoA.h"

namespace nc::physics
{
    ColliderSoA::ColliderSoA(size_t maxColliders)
        : handles(maxColliders), // braced init is ambiguous
          transforms{maxColliders},
          volumeData{maxColliders},
          types{maxColliders},
          gaps{},
          nextFree{0u}
    {
    }

    ColliderDataLocation::ColliderDataLocation()
        : handle{EntityHandle::Invalid()},
          index{0u},
          container{nullptr}
    {
    }

    ColliderDataLocation::ColliderDataLocation(EntityHandle handle_, uint32_t index_, ColliderSoA* container_)
        : handle{handle_},
          index{index_},
          container{container_}
    {
    }
} // namespace nc::physics