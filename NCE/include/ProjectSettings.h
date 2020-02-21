#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <string>

namespace nc
{
    struct DisplaySettings
    {
        int screenWidth = 0;
        int screenHeight = 0;
        int targetFPS = 0;
        double frameUpdateInterval = 0.0;
        double fixedUpdateInterval = 0.0;
    };

    struct InputBindings
    {

    };
    
    struct ProjectSettings
    {
        static std::string projectName;
        static DisplaySettings displaySettings;
        static InputBindings inputBindings;

        static void Load(const std::string &settingsFileName);
    };

    
}


#endif