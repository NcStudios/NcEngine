
#include "ncengine/graphics/NcGraphics.h"

#include <dlfcn.h>

namespace
{
auto CheckLibrary(const char* soName) -> bool
{
    void* handle = dlopen(soName, RTLD_LAZY);
    if (!handle) {
        return false;
    }
    dlclose(handle);
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
        supportedApis.reserve(1);

        if (CheckLibrary("libvulkan.so"))
            supportedApis.push_back(api::Vulkan);

        return supportedApis;
    }();

    return cachedApis;
}
} // namespace nc::graphics
