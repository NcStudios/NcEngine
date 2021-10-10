#pragma once

#include <stdexcept>

namespace nc
{
    template<class Service>
    class ServiceLocator
    {
        public:
            using service_type = Service;

            static void Register(service_type* service)
            {
                m_service = service;
            }

            static auto Get() -> service_type*
            {
                if(!m_service)
                    throw std::runtime_error("ServiceLocator - No service registered: " + std::string{__PRETTY_FUNCTION__});
                
                return m_service;
            }

        private:
            inline static service_type* m_service = nullptr;
    };
}