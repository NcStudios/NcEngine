#pragma once

#include "Component.h"

#include <string>
#include <string_view>

namespace nc
{
/**
 * @brief Component with a string tag.
 * 
 * @note Tags are automatically added and removed with their parent
 *       Entity. They do not need to be explicity created.
 * 
 * A Tag's initial value can be set in EntityInfo when adding an Entity.
 */
class Tag final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(Tag)

    public:
        Tag(Entity entity, std::string tag)
            : ComponentBase{entity},
              m_tag{std::move(tag)}
        {
        }

        /** @brief Set a new tag value. */
        void Set(std::string newTag)
        {
            m_tag = std::move(newTag);
        }

        /** @brief Get the tag value. */
        auto Value() const -> const std::string_view
        {
            return static_cast<std::string_view>(m_tag);
        }

    private:
        std::string m_tag;
};
} // namespace nc
