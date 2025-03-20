#pragma once

#include "asset/AssetService.h"

/**
 * Helper to define and create an IAssetService instance. The name of the instance is the provided
 * class name suffixed with 'Instance'. Methods trivially succeed/return true, except for 'Acquire`
 * which returns the value of the 'view' member. This is initially default constructed, but can be
 * set to another value at any point.
 * 
 * Example usage:
 *   DEFINE_ASSET_SERVICE_STUB(stubService, nc::asset::AssetType::AudioClip, nc::AudioClipView, std::string);
 */

#define DEFINE_ASSET_SERVICE_STUB(className, assetType, viewType, inputType)                                                                                               \
struct className : public nc::asset::IAssetService<viewType, inputType>                                                                                                    \
{                                                                                                                                                                          \
    viewType view;                                                                                                                                                         \
    const char* path = "test_path";                                                                                                                                        \
                                                                                                                                                                           \
    bool Load(const inputType&, nc::asset::AssetSubtype = nc::asset::AssetSubtype::None)                                                    override { return true; }      \
    bool Load(std::span<const inputType>, nc::asset::AssetSubtype = nc::asset::AssetSubtype::None)                                          override { return true; }      \
    bool Unload(const inputType&)                                                                                                           override { return true; }      \
    void UnloadAll()                                                                                                                        override {}                    \
    bool IsLoaded(const inputType&)                                                                  const                                  override { return true; }      \
    auto GetPath(nc::asset::AssetId)                                                                 const -> std::string_view              override { return path; }      \
    auto GetAllLoaded()                                                                              const -> std::vector<std::string_view> override { return {}; }        \
    auto GetAssetType()                                                                              const noexcept -> nc::asset::AssetType override { return assetType; } \
    auto Acquire(const inputType&)                                                                   const -> viewType override { return view; }                           \
    auto Acquire(nc::asset::AssetId)                                                                 const -> viewType override { return view; }                           \
}; className className##Instance;
