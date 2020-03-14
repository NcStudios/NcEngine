#pragma once
#include "NCWinDef.h"

namespace nc { class Transform; class EntityView; }

namespace nc::graphics { class Graphics; }

namespace nc::utils::editor
{
    class EditorManager
    {
        public:
            EditorManager(HWND hwnd, nc::graphics::Graphics& graphics);
            ~EditorManager() noexcept;

            LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            void BeginFrame();
            void EndFrame();

            void ToggleGui() noexcept;
            void EnableGui() noexcept;
            void DisableGui() noexcept;

            bool IsGuiActive() const noexcept;

            

            static void EntityControl(nc::EntityView* view);
            static void TransformGui(nc::Transform* transform);



            static void SpeedControl(float* speed);

            static void CameraControl();

            static void PointLightControl(float* xPos,      float* yPos,      float* zPos,
                                          float* ambientR,  float* ambientG,  float* ambientB,
                                          float* difColorR, float* difColorG, float* difColorB,
                                          float* difIntens, float* attConst,  float* attLin, float* attQuad);

            static bool BoxControl(int id, float* matX, float* specIntensity, float* specPower);

        private:
            bool m_isGuiActive;
    };
}