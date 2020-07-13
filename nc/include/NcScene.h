#pragma once
#include "scene/Scene.h"

#include <memory>

namespace nc::scene
{
    void NcChangeScene(std::unique_ptr<scene::Scene>&& scene);
}