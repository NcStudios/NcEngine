#pragma once
//#include <vector>
#include <memory>
//#include "NCWinDef.h"

//#include "Renderer.h"
#include "ComponentManager.h"
#include "PointLight.h"

//forwards
namespace nc
{
    //class Camera;

    namespace graphics 
    {
        class Graphics;
    }
}

namespace nc::engine
{
    class LightSystem : public ComponentManager<PointLight>
    {
        public:
            LightSystem();
            ~LightSystem();
            LightSystem(const LightSystem&) = delete;
            LightSystem(LightSystem&&) = delete;

            void BindLights(graphics::Graphics& graphics);

        private:
            //std::unique_ptr<graphics::Graphics>      m_graphics;
    };
} //end namespace nc::engine