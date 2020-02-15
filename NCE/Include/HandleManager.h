#ifndef HANDLE_MANAGER_H
#define HANDLE_MANAGER_H

namespace nc::internal
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

#endif