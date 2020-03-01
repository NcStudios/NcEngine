#pragma once

#include <vector>
#include <memory>
#include "NCWinDef.h"
//#include <windef.h> //HWND

//#include "Drawable.h"
//#include "Box2.h"
//#include "Box.h"

//forwards
namespace nc
{
    class Transform;
    class Camera;

    namespace graphics 
    {
        class PointLight;

        namespace internal
        {
            class Graphics;
            class Drawable;
        }
    }

    namespace editor
    {
        class EditorManager;
    }
}

namespace nc::internal
{
    class RenderingSystem
    {
        public:
            RenderingSystem(int initialWidth, int initialHeight, HWND hwnd);
            ~RenderingSystem();
            RenderingSystem(const RenderingSystem&) = delete;
            RenderingSystem(RenderingSystem&&) = delete;

            void StartRenderCycle(const std::vector<Transform> &transforms);
            nc::graphics::internal::Graphics& GetGraphics();

            void BindEditorManager(editor::EditorManager* editorManager);

        private:
            std::unique_ptr<Camera> m_camera;
            std::unique_ptr<graphics::PointLight> m_pointLight;

            std::unique_ptr<graphics::internal::Graphics> m_graphics;
            std::vector<std::unique_ptr<nc::graphics::internal::Drawable>> m_drawables;
            editor::EditorManager* m_editorManager;
            bool m_isGuiEnabled = true;
            void BeginFrame();
            void EndFrame();
    };
} //end namespace nc::internal