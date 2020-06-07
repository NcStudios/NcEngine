#ifdef NC_DEBUG
#pragma once
#include "win32/NCWinDef.h"
#include <optional>
#include <unordered_map>
#include "Entity.h"

namespace nc 
{
    class Transform;
    class EntityView;
    namespace graphics { class Graphics; }
}

namespace nc::utils::editor
{
    class EditorManager
    {
        public:
            std::optional<unsigned int> SelectedEntityIndex;

            EditorManager(HWND hwnd, nc::graphics::Graphics * graphics);
            ~EditorManager() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void BeginFrame();
            void Frame(float* dt, float frameLogicTime, uint32_t drawCallCount, std::unordered_map<EntityHandle, Entity>& activeEntities);
            void EndFrame();

            void ToggleGui() noexcept;
            void EnableGui() noexcept;
            void DisableGui() noexcept;
            bool IsGuiActive() const noexcept;

        private:
            bool m_isGuiActive;
            bool m_openState_EntityGraph       = true;
            bool m_openState_FramerateData     = true;
            bool m_openState_GraphicsResources = true;
            bool m_openState_ProjectSettings   = false;

            void DrawMenu();
            void DrawTimingControl(float* speed, float frameLogicTime, uint32_t drawCallCount, bool* open);
            void DrawEntityGraphControl(std::unordered_map<EntityHandle, Entity>& entities);
            void DrawInspectorControl(nc::EntityView* view);
    };
}
#endif