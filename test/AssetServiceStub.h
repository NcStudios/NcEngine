#pragma once

#include "assets/AssetService.h"

/**
 * Helper to define and create an IAssetService instance. The name of the instance is the provided
 * class name suffixed with 'Instance'. Methods trivially succeed/return true, except for 'Acquire`
 * which returns the value of the 'view' member. This is initially default constructed, but can be
 * set to another value at any point.
 * 
 * Example usage:
 *   DEFINE_ASSET_SERVICE_STUB(stubService, nc::asset::AssetType::AudioClip, nc::AudioClipView, std::string);
 */

#define DEFINE_ASSET_SERVICE_STUB(className, assetType, viewType, inputType)                                                \
struct className : public nc::IAssetService<viewType, inputType>                                                            \
{                                                                                                                           \
    viewType view;                                                                                                          \
                                                                                                                            \
    bool Load(const inputType&, bool, nc::asset_flags_type = nc::AssetFlags::None) override { return true; }                \
    bool Load(std::span<const inputType>, bool, nc::asset_flags_type = nc::AssetFlags::None) override { return true; }      \
    bool Unload(const inputType&, nc::asset_flags_type = nc::AssetFlags::None) override { return true; }                    \
    void UnloadAll(nc::asset_flags_type = nc::AssetFlags::None) override {}                                                 \
    bool IsLoaded(const inputType&, nc::asset_flags_type = nc::AssetFlags::None) const override { return true; }            \
    auto GetAllLoaded() const -> std::vector<std::string_view> override { return {}; }                                      \
    auto GetAssetType() const noexcept -> nc::asset::AssetType override { return assetType; }                               \
    auto Acquire(const inputType&, nc::asset_flags_type = nc::AssetFlags::None) const -> viewType override { return view; } \
}; className className##Instance;
