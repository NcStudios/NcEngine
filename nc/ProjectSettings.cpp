#include "ProjectSettings.h"

//#include <fstream>

namespace nc
{
    std::string ProjectSettings::projectName = "";
    DisplaySettings ProjectSettings::displaySettings = DisplaySettings();
    InputBindings ProjectSettings::inputBindings = InputBindings();

    void ProjectSettings::Load(const std::string &settingsFileName)
    {
        (void)settingsFileName;
        // std::string inputLine;

        // std::ifstream settingsFile;
        // settingsFile.open(settingsFileName);

        // std::getline(settingsFile, ProjectSettings::projectName);

        // std::getline(settingsFile, inputLine);
        // ProjectSettings::displaySettings.screenWidth = std::stoi(inputLine);

        // std::getline(settingsFile, inputLine);
        // ProjectSettings::displaySettings.screenHeight = std::stoi(inputLine);

        // std::getline(settingsFile, inputLine);
        // ProjectSettings::displaySettings.targetFPS = std::stoi(inputLine);

        // ProjectSettings::displaySettings.frameUpdateInterval = 1.0 / ProjectSettings::displaySettings.targetFPS;

        // std::getline(settingsFile, inputLine);
        // ProjectSettings::displaySettings.fixedUpdateInterval = std::stod(inputLine); 

        // settingsFile.close();

        ProjectSettings::projectName = "MyProjectName";
        ProjectSettings::displaySettings.screenWidth = 1500;
        ProjectSettings::displaySettings.screenHeight = 1200;
        ProjectSettings::displaySettings.targetFPS = 60;
        ProjectSettings::displaySettings.frameUpdateInterval = 1.0 / ProjectSettings::displaySettings.targetFPS;
        ProjectSettings::displaySettings.fixedUpdateInterval = 0.2;
    }
}