#pragma once

#include "Assets.h"
#include "service/ServiceLocator.h"

namespace nc
{
    /** Interface for services that manage assets. */
    template<AssetType T, class InputType>
    class IAssetService
    {
        public:
            using data_type = T;

            IAssetService();
            virtual ~IAssetService() = default;

            virtual bool Load(const InputType& input) = 0;
            virtual bool Load(std::span<const InputType> inputs) = 0;
            virtual bool Unload(const InputType& input) = 0;
            virtual void UnloadAll() = 0;
            virtual auto Acquire(const InputType& input) const -> data_type = 0;
            virtual bool IsLoaded(const InputType& input) const = 0;
    };

    /** Helper alias for locating asset services. */
    template<AssetType T, class InputType = std::string>
    using AssetService = ServiceLocator<IAssetService<T, InputType>>;

    template<AssetType T, class InputType>
    IAssetService<T, InputType>::IAssetService()
    {
        AssetService<T, InputType>::Register(this);
    }
} // namespace nc