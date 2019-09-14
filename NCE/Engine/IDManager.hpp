


namespace NCE::Engine
{
    class IDManager
    {
        private:
            int _currentID = 0;
        
        public:
            int GetNewEntityID() { return _currentID++; }
            void Reset() { _currentID = 0; }
    };
}