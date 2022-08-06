#pragma once

#include "service/ServiceLocator.h"

#include <vector>

namespace nc
{
    struct EnvironmentData;
    struct PointLightInfo;
    namespace graphics
    {
        struct ShadowMap;
        class Graphics;
        struct ObjectData;
        struct TextureBuffer;
        class CubeMap;
    }
}

namespace vk
{
    class DescriptorSetLayout;
}

namespace nc::graphics
{
    /** Restrict instantiations to supported shader parameters to minimize
     *  errors with the service locator. */
    template<class T>
    concept ShaderResource = std::same_as<T, ObjectData> ||
                             std::same_as<T, PointLightInfo> ||
                             std::same_as<T, TextureBuffer> ||
                             std::same_as<T, ShadowMap> || 
                             std::same_as<T, EnvironmentData> ||
                             std::same_as<T, CubeMap>;

    /** Interface for types that manage shader parameters. */
    template<ShaderResource T>
    class IShaderResourceService
    {
        public:
            using data_type = T;

            IShaderResourceService();
            virtual ~IShaderResourceService() = default;
            
            virtual void Initialize() = 0;
            virtual void Update(const std::vector<data_type>& data) = 0;
            virtual void Reset() = 0;
    };

    /** Helper alias for locating shader resource services. */
    template<ShaderResource T>
    using ShaderResourceService = ServiceLocator<IShaderResourceService<T>>;

    template<ShaderResource T>
    IShaderResourceService<T>::IShaderResourceService()
    {
        ShaderResourceService<T>::Register(this);
    }
}