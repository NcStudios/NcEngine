#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/PhongShadingTechnique.h"
#include "graphics/techniques/WireframeTechnique.h"
#include "graphics/techniques/TechniqueType.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace nc::graphics
{
    class TechniqueManager
    {
        public:
            template<std::derived_from<Technique> T, class ...Args>
            static Technique* GetTechnique(Args&& ... args);

        private:
            template<std::derived_from<Technique> T, class ...Args>
            static size_t GetUID(Args&& ... args);
            
            template<std::derived_from<Technique> T, class ...Args>
            static std::unique_ptr<Technique> AddTechnique(Args&& ... args);
            static std::unordered_map<size_t, std::unique_ptr<Technique>> m_techniqueMap;
    };

    template<std::derived_from<Technique> T, class ...Args>
    Technique* TechniqueManager::GetTechnique(Args&& ... args)
    {
        auto uid = GetUID<T>(std::forward<Args>(args)...);
        auto technique = m_techniqueMap.find(uid);
        if (technique == m_techniqueMap.end())
        {
            m_techniqueMap.emplace(uid, AddTechnique<T>(std::forward<Args>(args)...));
            return m_techniqueMap.at(uid).get();
        }
        else
        {
            return technique->second.get();
        }
    }

    template<std::derived_from<Technique> T, class ...Args>
    std::unique_ptr<Technique> TechniqueManager::AddTechnique(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<std::derived_from<Technique> T, class ...Args>
    size_t TechniqueManager::GetUID(Args&& ... args)
    {
        return T::GetUID(std::forward<Args>(args)...);
    }
}
