


namespace NCE::Engine
{
    class IDManager
    {
        private:
            int m_currentID = 0;
        
        public:
            int GetNewEntityID() { return m_currentID++; }
            void Reset() { m_currentID = 0; }
    };
}