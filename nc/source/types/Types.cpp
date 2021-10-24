
#include "types/Types.h"

namespace nc
{

    /** Types Initialization */
    std::map<Key, TypeBase*>& Types::GetMap()
    {
        static std::map<Key, TypeBase*> m_map;

        return m_map;
    }

    std::vector<TypeBase*>& Types::GetList()
    {
        static std::vector<TypeBase*> m_list;

        return m_list;
    }

    TypeBase* Types::Get(const Key& name)
    {
        auto& map = GetMap();

        return map[name];
    }
    
    std::vector<TypeBase*> Types::Matches(MatchCondition match)
    {
        auto& list = GetList();
        std::vector<TypeBase*> matches;

        for (auto& type : list)
        {
            if (match(type->GetFlags()))
            {
                matches.push_back(type);
            }
        }

        return matches;
    }

}