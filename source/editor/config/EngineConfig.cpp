#include "EngineConfig.h"

#include "ncengine/config/Config.h"

namespace nc::editor
{
auto MakeEngineConfig() -> config::Config
{
    auto config = config::Config{};
    config.projectSettings.projectName = "NcEditor";
    config.projectSettings.logFilePath = "nceditor.log";

    config.assetSettings.audioClipsPath = "../resources/assets/nca/"; // dummy path
    config.assetSettings.concaveCollidersPath = "../resources/assets/nca/concave_colliders/";
    config.assetSettings.hullCollidersPath = "../resources/assets/nca/hull_colliders/";
    config.assetSettings.meshesPath = "../resources/assets/nca/meshes/";
    config.assetSettings.texturesPath = "../resources/assets/nca/texture/";
    config.assetSettings.cubeMapsPath = "../resources/assets/nca/cube_maps";
    config.assetSettings.shadersPath = "../resources/assets/shaders/compiled/";

    config.audioSettings.enabled = false;
    config.physicsSettings.enabled = false;
    return config;
}
} // namespace nc::editor
