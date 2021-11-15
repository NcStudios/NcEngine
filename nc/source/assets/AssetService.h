#pragma once

#include "Assets.h"
#include "service/ServiceLocator.h"

namespace nc
{
    /** Interface for services that manage assets. */
    template<AssetType T>
    class IAssetService
    {
        public:
            using data_type = T;

            IAssetService();
            virtual ~IAssetService() = default;

            virtual bool Load(const std::string& path) = 0;
            virtual bool Load(std::span<const std::string> paths) = 0;
            virtual bool Unload(const std::string& path) = 0;
            virtual void UnloadAll() = 0;
            virtual auto Acquire(const std::string& path) const -> data_type = 0;
            virtual bool IsLoaded(const std::string& path) const = 0;
    };

    /** Helper alias for locating asset services. */
    template<AssetType T>
    using AssetService = ServiceLocator<IAssetService<T>>;

    template<AssetType T>
    IAssetService<T>::IAssetService()
    {
        AssetService<T>::Register(this);
    }
} // namespace nc