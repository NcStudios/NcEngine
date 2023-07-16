#include "NcEditor.h"
#include "ui/EditorUI.h"

#include "ncengine/NcEngine.h"
#include "ncengine/graphics/NcGraphics.h"

namespace nc::editor
{
NcEditor::NcEditor(NcEngine* engine)
    : m_engine{engine},
      m_mainUI{std::make_unique<EditorUI>()}
{
    engine->GetModuleRegistry()->Get<graphics::NcGraphics>()->SetUi(m_mainUI.get());
}

NcEditor::~NcEditor() noexcept = default;
} // namespace nc::editor
