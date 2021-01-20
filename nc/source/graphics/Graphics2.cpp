#include "Graphics2.h"

namespace nc::graphics
{
    Graphics2::Graphics2()
    {
        m_instance = vulkan::Instance::Create();
    }
}