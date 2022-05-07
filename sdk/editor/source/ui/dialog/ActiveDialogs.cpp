#include "ActiveDialogs.h"

namespace nc::editor
{
    void ActiveDialogs::Register(DialogBase* dialog)
    {
        m_toAdd.push_back(dialog);
    }
    
    void ActiveDialogs::Draw()
    {
        m_dialogs.insert(m_dialogs.end(), m_toAdd.begin(), m_toAdd.end());
        m_toAdd.clear();

        for(auto& dialog : m_dialogs)
        {
            if(!dialog->IsOpen())
                dialog = nullptr;
            else
                dialog->Draw();
        }

        std::erase(m_dialogs, nullptr);
    }
}