#include "SceneDeleter.h"
#include "utility/Output.h"

namespace nc::editor
{
    void DeleteScene(const std::filesystem::path& scenesDirectory, const std::string& sceneName)
    {
        try
        {
            Output::Log("Deleting Scene: " + sceneName);

            auto headerPath = scenesDirectory / (sceneName + FileExtension::Header.data());
            auto sourcePath = scenesDirectory / (sceneName + FileExtension::Source.data());
            auto genPath = scenesDirectory / (sceneName + FileExtension::Generated.data());

            if(!std::filesystem::remove(headerPath))
                Output::LogError("Failure deleting: " + headerPath.string());

            if(!std::filesystem::remove(sourcePath))
                Output::LogError("Failure deleting: " + headerPath.string());

            if(!std::filesystem::remove(genPath))
                Output::LogError("Failure deleting: " + genPath.string());
        }
        catch(const std::filesystem::filesystem_error& e)
        {
            Output::LogError("Failure deleting scene:", std::string{"Exception: "} + std::string{e.what()});
        }
    }
}