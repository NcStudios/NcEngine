#include "ecs/component/AttachmentGroup.h"

#include <algorithm>

namespace nc
{
    AttachmentGroup::AttachmentGroup(Entity)
        : m_attachments{},
          m_toAdd{},
          m_toRemove{}
    {}

    auto AttachmentGroup::GetAttachments() const noexcept -> std::vector<StateAttachment*>
    {
        std::vector<StateAttachment*> out(m_attachments.size());
        std::transform(m_attachments.cbegin(), m_attachments.cend(), out.begin(), [](const auto& ptr)
        {
            return ptr.get();
        });
        return out;
    }

    void AttachmentGroup::CommitStagedAttachments()
    {
        for(auto i : m_toRemove)
        {
            m_attachments.at(i) = std::move(m_attachments.back());
            m_attachments.pop_back();
        }

        m_toRemove.clear();

        for(auto& toAdd : m_toAdd)
            m_attachments.push_back(std::move(toAdd));

        m_toAdd.clear();
    }
}