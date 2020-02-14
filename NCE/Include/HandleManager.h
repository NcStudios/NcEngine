#ifndef HANDLE_MANAGER
#define HANDLE_MANAGER

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


#endif