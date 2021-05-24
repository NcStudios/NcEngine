#pragma once

#include "Ecs.h"

// namespace nc
// {
//     template<class T>
//     class ComponentView
//     {
//         public:
//             ComponentView(T* ptr, EntityHandle handle)
//                 : m_ptr{ptr}, m_handle{handle}
//             {}

//             T* operator->()
//             {
//                 auto currentVersion = SystemVersion<T>();
//                 if(m_handle.Version() == currentVersion)
//                     return m_ptr;
                
//                 m_handle.SetVersion(currentVersion);
//                 m_ptr = GetComponent<T>(m_handle);
//                 return m_ptr;
//             }

//         private:
//             T* m_ptr;
//             EntityHandle m_handle;
//     };
// }