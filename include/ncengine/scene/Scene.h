#pragma once

#include "ncengine/module/ModuleProvider.h"

#include <filesystem>
#include <memory>

namespace nc
{
class Registry;

class Scene
{
    public:
        virtual ~Scene() = default;
        virtual void Load(Registry* registry, ModuleProvider modules) = 0;
        virtual void Unload() {}
};

class DynamicScene : public Scene
{
    public:
        explicit DynamicScene(std::filesystem::path fragmentPath)
            : m_fragmentPath{std::move(fragmentPath)}
        {
        }

        void Load(Registry* registry, ModuleProvider modules) override;

    private:
        std::filesystem::path m_fragmentPath;
};
} // namespace nc
