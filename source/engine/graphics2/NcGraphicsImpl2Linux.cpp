
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
auto GetSupportedApis() -> std::vector<std::string_view>
{
    auto supportedApis = std::vector<std::string_view>{};
    supportedApis.reserve(2);
    supportedApis.push_back(api::OpenGL);

    if (CheckLibrary("libvulkan.so"))
        supportedApis.push_back(api::Vulkan);
    
    return supportedApis;
}
} // namespace nc::graphics
