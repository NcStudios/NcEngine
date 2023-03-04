#pragma once

#include "service/ServiceLocator.h"

#include <vector>

namespace nc::graphics
{
/** Interface for types that manage shader parameters. */
template<class T>
class IShaderResource
{
    public:
        using data_type = T;

        IShaderResource();
        virtual ~IShaderResource() = default;
        
        virtual void Initialize() = 0;
        virtual void Update(const std::vector<data_type>& data) = 0;
        virtual void Reset() = 0;
};

/** Helper alias for locating shader resource services. */
template<class T>
using ShaderResourceService = ServiceLocator<IShaderResource<T>>;

template<class T>
IShaderResource<T>::IShaderResource()
{
    ShaderResourceService<T>::Register(this);
}
} // namespace nc::graphics