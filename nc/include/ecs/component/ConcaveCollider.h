#pragma once

#include "Component.h"

namespace nc
{
    class ConcaveCollider final : public ComponentBase
    {
        NC_ENABLE_IN_EDITOR(ConcaveCollider)
        
        public:
            ConcaveCollider(Entity entity, std::string assetPath);
            auto GetPath() const -> const std::string& { return m_path; }

        private:
            std::string m_path;
    };

    template<>
    struct StoragePolicy<ConcaveCollider>
    {
        using allow_trivial_destruction = std::false_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ConcaveCollider>(ConcaveCollider*);
    #endif
}