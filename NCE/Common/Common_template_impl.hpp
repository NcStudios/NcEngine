#ifndef NCE_COMMON_COMMON_T_IMPL
#define NCE_COMMON_COMMON_T_IMPL

#include <typeinfo>

namespace NCE::Common
{
    template<class T>
    void Entity::AddComponent(EntityWeakPtr parent_)
    {
        //make sure only one component of a type is attached?

        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component.");


        // auto newComponent = std::make_shared<T>(t_parent);
        // auto castComponent = std::dynamic_pointer_cast<Component>(newComponent);
        // _components.push_back(castComponent);

        m_components.push_back( std::dynamic_pointer_cast<Component>(std::make_shared<T>(parent_)) );
    }


    template<class T>
    std::weak_ptr<T> Entity::GetComponent(uint32_t componentId_)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component.");


        for(auto& current : m_components)
        {
            if (componentId_ == current->TypeId)
            {
                //static cast?
                
                return std::dynamic_pointer_cast<T>(current);
            }
            
        }

        //std::cout << "Entity::GetComponent() - component not found";
        //this is probably a bad idea
        return std::weak_ptr<T>();
    }
}


#endif