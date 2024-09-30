#include "ncengine/graphics/NcGraphics.h"

#include <windows.h>
#include <string_view>
#include <vector>

namespace
{
auto CheckLibrary(char const* dllName) -> bool
{
    auto hModule = LoadLibrary(dllName);
    if (hModule == NULL) {
        return false;
    }
    FreeLibrary(hModule);
    return true;
}
} // anonymous namespace

namespace nc::graphics
{
auto GetSupportedApis() -> std::vector<std::string_view>
{
    auto supportedApis = std::vector<std::string_view>{};
    supportedApis.reserve(4);
    supportedApis.push_back(api::OpenGL);

    if (CheckLibrary("d3d12.dll"))
        supportedApis.push_back(api::D3D12);

    if (CheckLibrary("vulkan-1.dll"))
        supportedApis.push_back(api::Vulkan);

    if (CheckLibrary("d3d11.dll"))
        supportedApis.push_back(api::D3D11);
    
    return supportedApis;
}
} // namespace nc::graphics
