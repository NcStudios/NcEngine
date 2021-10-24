#pragma once

#include <string>
#include <string_view>

namespace nc
{
    class Tag final
    {
        public:
            Tag(Entity, std::string tag)
                : m_tag{std::move(tag)}
            {}

            Tag(Tag&&) = default;
            Tag& operator=(Tag&&) = default;
            ~Tag() = default;

            Tag(const Tag&) = delete;
            Tag& operator=(const Tag&) = delete;

            auto Value() const -> const std::string_view
            {
                return static_cast<std::string_view>(m_tag);
            }

        private:
            std::string m_tag;
    };
}