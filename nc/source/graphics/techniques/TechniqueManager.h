#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/PhongShadingTechnique.h"
#include "graphics/techniques/TechniqueType.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace nc::graphics
{
    class TechniqueManager
    {
        public:
            template<class ... Args>
            static Technique* GetTechnique(TechniqueType type, Args&& ... args);

        private:
            template<class ... Args>
            static size_t GetUID(TechniqueType type, Args&& ... args);
            
            template<class ... Args>
            static std::unique_ptr<Technique> AddTechnique(TechniqueType type, Args&& ... args);
            static std::unordered_map<size_t, std::unique_ptr<Technique>> m_techniqueMap;
    };

    template<class ... Args>
    Technique* TechniqueManager::GetTechnique(TechniqueType type, Args&& ... args)
    {
        auto uid = GetUID(type, std::forward<Args>(args)...);
        auto technique = m_techniqueMap.find(uid);
        if (technique == m_techniqueMap.end())
        {
            m_techniqueMap.emplace(uid, AddTechnique(type,std::forward<Args>(args)...));
            return m_techniqueMap.at(uid).get();
        }
        else
        {
            return technique->second.get();
        }
    }

    template<class ... Args>
    std::unique_ptr<Technique> TechniqueManager::AddTechnique(TechniqueType type, Args&& ... args)
    {
        switch (type)
        {
            case TechniqueType::PhongShadingTechnique:
                return std::make_unique<PhongShadingTechnique>(std::forward<Args>(args)...);
            default:
                throw std::runtime_error("Tried to create a technique with a TechniqueType that does not exist.");
        }
    }

    template<class ... Args>
    size_t TechniqueManager::GetUID(TechniqueType type, Args&& ... args)
    {
        switch (type)
        {
            case TechniqueType::PhongShadingTechnique:
                return PhongShadingTechnique::GetUID(type, std::forward<Args>(args)...);
            default:
                throw std::runtime_error("Tried to get the UID of a TechniqueType that does not exist.");
        }
    }
}
