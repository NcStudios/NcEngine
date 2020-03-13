#pragma once

#include <vector>
#include <memory>
#include "NCWinDef.h"
//#include <windef.h> //HWND

//#include "Drawable.h"
//#include "Box2.h"
//#include "Box.h"

#include "HandleManager.h"
#include <set>
#include "Renderer.h"
#include "ComponentManager.h"

//forwards
namespace nc
{
    class Transform;
    class Camera;

    namespace graphics 
    {
        class PointLight;
        class Graphics;
        namespace d3dresource { class Drawable; }
        namespace primitive { class Box; }
    }

    namespace utils::editor { class EditorManager; }
}

namespace nc::engine
{
    class RenderingSystem : public ComponentManager<Renderer>
    {
        public:
            RenderingSystem(int initialWidth, int initialHeight, HWND hwnd);
            ~RenderingSystem();
            RenderingSystem(const RenderingSystem&) = delete;
            RenderingSystem(RenderingSystem&&) = delete;

            void StartRenderCycle(const std::vector<Transform> &transforms);
            nc::graphics::Graphics& GetGraphics();

            void BindEditorManager(utils::editor::EditorManager* editorManager);

        private:
            //Camera& m_camera;
            std::unique_ptr<graphics::PointLight> m_pointLight;

            std::unique_ptr<graphics::Graphics> m_graphics;
            //std::vector<nc::Renderer> m_renderers;


            //std::vector<nc::Renderer*> m_rendererPointers; 
            std::optional<unsigned int> m_comboBoxIndex;
            std::set<unsigned int> m_boxControlIds;

            utils::editor::EditorManager* m_editorManager;
            bool m_isGuiEnabled = true;
            void BeginFrame();
            void EndFrame();
    };
} //end namespace nc::internal