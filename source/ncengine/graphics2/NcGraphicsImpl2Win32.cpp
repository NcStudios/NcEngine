#include "ncengine/graphics/NcGraphics.h"
#include "ncutility/platform/win32/NcWin32.h"

#include <string_view>
#include <vector>

namespace
{
auto CheckLibrary(char const* dllName) -> bool
{
    auto hModule = LoadLibraryExA(dllName, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hModule == NULL) {
        return false;
    }
    FreeLibrary(hModule);
    return true;
}
} // anonymous namespace

namespace nc::graphics
{
auto GetSupportedApis() -> std::span<const std::string_view>
{
    static const auto cachedApis = []()
    {
        auto supportedApis = std::vector<std::string_view>{};
        supportedApis.reserve(2);

        if (CheckLibrary("d3d12.dll"))
            supportedApis.push_back(api::D3D12);

        if (CheckLibrary("vulkan-1.dll"))
            supportedApis.push_back(api::Vulkan);

        return supportedApis;
    }();
    
    return cachedApis;
}
} // namespace nc::graphics
