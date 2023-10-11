#pragma once

#include "asset/Assets.h"
#include "service/ServiceLocator.h"

#include <string_view>
#include <vector>

namespace nc
{
    /** Interface for services that manage assets. */
    template<AssetView T, class InputType>
    class IAssetService
    {
        public:
            using data_type = T;

            IAssetService();
            virtual ~IAssetService() = default;

            virtual bool Load(const InputType& input, bool isExternal, asset_flags_type flags = AssetFlags::None) = 0;
            virtual bool Load(std::span<const InputType> inputs, bool isExternal, asset_flags_type flags = AssetFlags::None) = 0;
            virtual bool Unload(const InputType& input, asset_flags_type flags = AssetFlags::None) = 0;
            virtual void UnloadAll(asset_flags_type flags = AssetFlags::None) = 0;
            virtual auto Acquire(const InputType& input, asset_flags_type flags = AssetFlags::None) const -> data_type = 0;
            virtual bool IsLoaded(const InputType& input, asset_flags_type flags = AssetFlags::None) const = 0;
            virtual auto GetAllLoaded() const -> std::vector<std::string_view> = 0;
    };

    /** Helper alias for locating asset services. */
    template<AssetView T, class InputType = std::string>
    using AssetService = ServiceLocator<IAssetService<T, InputType>>;

    template<AssetView T, class InputType>
    IAssetService<T, InputType>::IAssetService()
    {
        AssetService<T, InputType>::Register(this);
    }
} // namespace nc
