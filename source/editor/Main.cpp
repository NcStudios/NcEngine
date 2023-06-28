#include "NcEditor.h"
#include "EditorScene.h"
#include "config/EngineConfig.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

int main()
{
    auto engine = std::unique_ptr<nc::NcEngine>{};

    try
    {
        const auto config = nc::editor::MakeEngineConfig();
        engine = nc::InitializeNcEngine(config);
        auto editor = nc::editor::NcEditor{engine.get()};
        engine->Start(std::make_unique<nc::editor::EditorScene>());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        NC_LOG_EXCEPTION(e);
    }
    catch(...)
    {
        std::cerr << "Unknown exception\n";
        NC_LOG_ERROR("Unknown exception");
    }

    if (engine)
    {
        engine->Shutdown();
    }

    return 0;
}
