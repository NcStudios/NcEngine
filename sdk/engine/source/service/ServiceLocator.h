#pragma once

#include "debug/NcError.h"

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
                    throw NcError("No service registered");
                
                return m_service;
            }

        private:
            inline static service_type* m_service = nullptr;
    };
}