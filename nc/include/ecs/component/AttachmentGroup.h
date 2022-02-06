#pragma once

#include "Component.h"
#include "debug/Utils.h"

#include <concepts>
#include <memory>
#include <set>
#include <vector>

namespace nc
{
    class AttachmentGroup final
    {
        public:
            AttachmentGroup(Entity entity);
            ~AttachmentGroup() = default;
            AttachmentGroup(AttachmentGroup&&) = default;
            AttachmentGroup& operator=(AttachmentGroup&&) = default;
            AttachmentGroup(const AttachmentGroup&) = delete;
            AttachmentGroup& operator=(const AttachmentGroup&) = delete;

            template<std::derived_from<StateAttachment> T, class ... Args>
            T* Add(Args&& ... args);

            template<std::derived_from<StateAttachment> T>
            void Remove();

            template<std::derived_from<StateAttachment> T>
            bool Contains() const noexcept;

            template<std::derived_from<StateAttachment> T>
            T* Get() const noexcept; 

            auto GetAttachments() const noexcept -> std::vector<StateAttachment*>;
            void CommitStagedAttachments();

        private:
            std::vector<std::unique_ptr<StateAttachment>> m_attachments;
            std::vector<std::unique_ptr<StateAttachment>> m_toAdd;
            std::set<size_t> m_toRemove;
    };

    template<std::derived_from<StateAttachment> T, class ... Args>
    T* AttachmentGroup::Add(Args&& ... args)
    {
        IF_THROW(Contains<T>(), "Entity already has an attachment of this type");
        auto attachment = std::make_unique<T>(std::forward<Args>(args)...);
        auto* out = attachment.get();
        m_toAdd.push_back(std::move(attachment));
        return out;
    }

    template<std::derived_from<StateAttachment> T>
    void AttachmentGroup::Remove()
    {
        for(size_t i = 0; auto& attachment : m_attachments)
        {
            if(dynamic_cast<T*>(attachment.get()))
            {
                m_toRemove.insert(i);
                return;
            }

            ++i;
        }

        throw NcError("Attachment does not exist");
    }

    template<std::derived_from<StateAttachment> T>
    bool AttachmentGroup::Contains() const noexcept
    {
        return Get<T>() != nullptr;
    }

    template<std::derived_from<StateAttachment> T>
    T* AttachmentGroup::Get() const noexcept
    {
        for(auto& item : m_attachments)
        {
            auto* ptr = dynamic_cast<T*>(item.get());
            if(ptr)
                return ptr;
        }

        for(auto& item : m_toAdd)
        {
            auto* ptr = dynamic_cast<T*>(item.get());
            if(ptr)
                return ptr;
        }

        return nullptr;
    }
}