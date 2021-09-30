#include "SceneDeleter.h"
#include "utility/Output.h"

namespace nc::editor
{
    void DeleteScene(const std::filesystem::path& scenesDirectory, const std::string& sceneName)
    {
        try
        {
            Output::Log("Deleting Scene: " + sceneName);

            auto headerPath = scenesDirectory / (sceneName + HeaderExtension);
            auto sourcePath = scenesDirectory / (sceneName + SourceExtension);
            auto genPath = scenesDirectory / (sceneName + GeneratedSourceExtension);

            if(!std::filesystem::remove(headerPath))
                Output::Log("Failure deleting: " + headerPath.string());

            if(!std::filesystem::remove(sourcePath))
                Output::Log("Failure deleting: " + headerPath.string());

            if(!std::filesystem::remove(genPath))
                Output::Log("Failure deleting: " + genPath.string());
        }
        catch(const std::filesystem::filesystem_error& e)
        {
            Output::Log("Exception throw deleting scene: " + std::string{e.what()});
        }
    }
}