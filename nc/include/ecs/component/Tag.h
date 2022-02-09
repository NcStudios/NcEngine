#pragma once

#include "ecs/component/Component.h"

#include <string>
#include <string_view>

namespace nc
{
    class Tag final : public ComponentBase
    {
        public:
            Tag(Entity entity, std::string tag)
                : ComponentBase{entity},
                  m_tag{std::move(tag)}
            {
            }

            void Set(std::string newTag)
            {
                m_tag = std::move(newTag);
            }

            auto Value() const -> const std::string_view
            {
                return static_cast<std::string_view>(m_tag);
            }

        private:
            std::string m_tag;
    };
}