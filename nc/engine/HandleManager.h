#pragma once

namespace nc::engine
{
    template<class T>
    class HandleManager
    {
        private:
            T m_current;
        public:
            HandleManager() : m_current(0) {};
            T GenerateNewHandle()
            {
                return m_current++;
            }
    };
}