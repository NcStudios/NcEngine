#include "TechniqueManager.h"

namespace nc::graphics
{
    std::unordered_map<size_t, std::unique_ptr<Technique>> TechniqueManager::m_techniqueMap = {};
}