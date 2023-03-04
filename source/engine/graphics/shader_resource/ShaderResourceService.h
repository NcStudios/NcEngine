#pragma once

#include "service/ServiceLocator.h"

#include <vector>

namespace nc
{
namespace graphics
{
class CubeMap;
struct EnvironmentData;
struct PointLightInfo;
struct ObjectData;
struct ShadowMap;
struct TextureBuffer;
} // namespace graphics
} // namespace nc

namespace vk
{
    class DescriptorSetLayout;
} // namespace vk

namespace nc::graphics
{
/** Restrict instantiations to supported shader parameters to minimize
 *  errors with the service locator. */
template<class T>
concept ShaderResourceElement = std::same_as<T, ObjectData>      ||
                                std::same_as<T, PointLightInfo>  ||
                                std::same_as<T, TextureBuffer>   ||
                                std::same_as<T, ShadowMap>       ||
                                std::same_as<T, EnvironmentData> ||
                                std::same_as<T, CubeMap>;

/** Interface for types that manage shader parameters. */
template<ShaderResourceElement T>
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
template<ShaderResourceElement T>
using ShaderResourceService = ServiceLocator<IShaderResource<T>>;

template<ShaderResourceElement T>
IShaderResource<T>::IShaderResource()
{
    ShaderResourceService<T>::Register(this);
}
} // namespace nc::graphics