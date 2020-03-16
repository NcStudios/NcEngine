#pragma once
#include "NCWinDef.h"
#include <optional>

namespace nc { class Transform; class EntityView; }

namespace nc::graphics { class Graphics; }

namespace nc::utils::editor
{
    class EditorManager
    {
        public:
            std::optional<unsigned int> SelectedEntityIndex;

            EditorManager(HWND hwnd, nc::graphics::Graphics& graphics);
            ~EditorManager() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void BeginFrame();
            void EndFrame();

            void ToggleGui() noexcept;
            void EnableGui() noexcept;
            void DisableGui() noexcept;

            bool IsGuiActive() const noexcept;

            

            static bool EntityControl(nc::EntityView* view);


            static void SpeedControl(float* speed);

            static void PointLightControl(float* xPos,      float* yPos,      float* zPos,
                                          float* ambientR,  float* ambientG,  float* ambientB,
                                          float* difColorR, float* difColorG, float* difColorB,
                                          float* difIntens, float* attConst,  float* attLin, float* attQuad);

        private:
            bool m_isGuiActive;
    };
}