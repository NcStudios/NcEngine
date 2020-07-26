#pragma once

namespace nc::engine::system
{
template<class T>
class HandleManager
{
    private:
        T m_current;
    public:
        HandleManager() : m_current(1)
        {
        }

        T GenerateNewHandle()
        {
            return m_current++;
        }

        T GetCurrent() const
        {
            return m_current;
        }

        void Reset()
        {
            m_current = 1;
        }
};
}