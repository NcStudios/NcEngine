#pragma once

#ifdef NC_EDITOR_ENABLED
namespace nc
{
    class UIStream;

    namespace editor
    {
        struct AssetDependencyChecker;
        class Inspector;
    }
}
#define NC_ENABLE_IN_EDITOR(Component)                        \
public:                                                       \
    static constexpr auto NcEditorComponentName = #Component; \
private:                                                      \
    friend ::nc::editor::Inspector;                           \
    friend ::nc::editor::AssetDependencyChecker;              \
    friend ::nc::UIStream;
#else
#define NC_ENABLE_IN_EDITOR(Component)
#endif
