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
    struct StoragePolicy<ConcaveCollider> : DefaultStoragePolicy
    {
        static constexpr bool EnableOnAddCallbacks = true;
        static constexpr bool EnableOnRemoveCallbacks = true;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ConcaveCollider>(ConcaveCollider*);
    #endif
}