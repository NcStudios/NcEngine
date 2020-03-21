#pragma once

namespace nc::engine
{
    template<class T>
    class HandleManager
    {
        private:
            T m_current;
        public:
            HandleManager() : m_current(1) {};
            T GenerateNewHandle()
            {
                return m_current++;
            }

            T GetCurrent() 
            {
                return m_current;
            }
    };
}