#pragma once

#include <memory>

namespace nc
{
class NcEngine;

namespace editor
{
class EditorUI;

class NcEditor
{
    public:
        NcEditor(NcEngine* engine);
        ~NcEditor() noexcept;

        NcEditor(const NcEditor&) = delete;
        NcEditor(NcEditor&&) = delete;
        NcEditor& operator=(const NcEditor&) = delete;
        NcEditor& operator=(NcEditor&&) = delete;

    private:
        NcEngine* m_engine;
        std::unique_ptr<EditorUI> m_mainUI;
};
} // namespace editor
} // namespace nc
