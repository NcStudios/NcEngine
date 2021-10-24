#pragma once

#include <functional>

#include "utility/Key.h"
#include "utility/Flags.h"
#include "utility/Match.h"
#include "types/Property.h"
#include "types/Access.h"


namespace nc
{

    using TypeId = uint16_t;

    /**
     * Base struct for types. Has a name and a function which can create a shared_ptr to a value of its type.
     */
    class TypeBase
    {
        using create_type = std::function<std::shared_ptr<void>()>;
    public:
        TypeBase(const TypeId id, const Key& name, create_type& creator)
            : m_id(id), m_name(name), m_creator(creator), m_flags(0) {};

        // Creates a shared_ptr with a value of this type.
        std::shared_ptr<void> Create() const { return m_creator(); };

        // The name of the type. Should be unique.
        const Key& Name() const { return m_name; };

        // The flags assigned to this type.
        Flags& GetFlags() { return m_flags; };

    protected:
        const TypeId m_id;
        const Key m_name;
        const create_type m_creator;
        Flags m_flags;
    };

    /**
     * A type describes system and user specified types and their properties. Types can have static properties and
     * dynamic properties that are built at runtime based on an instance of the type. Properties can be read-only,
     * write-only, read and write, or can be computed upon request.
     *
     * You can also expose internal values of a class to its type by using a friend function.
     *
     * ```
     * class MyObject {
     * public:
     *   // .. methods
     * private:
     *   int m_key;
     *
     *   template <> friend void InitType(Type<MyObject>* t) {
     *     // Add as read-only property
     *     t->Add<int>("key", types::Int, {
     *       .get = [](MyObject* i, int* out) { *out = i->m_key; return true }
     *     });
     *   }
     * }
     *
     * // this defines the type and will call the InitType defined above automatically
     * namespace types {
     *   auto Int      = Types::Define<int>("int");
     *   auto MyObject = Types::Define<::MyObject>("MyObject");
     * }
     * ```
     *
     * @tparam T The type type.
     */
    template <typename T>
    class Type : public TypeBase
    {
    public:
        Type(const TypeId id, const Key& name, std::function<std::shared_ptr<void>()> creator)
            : TypeBase(id, name, creator) {}

        Type(const Type<T>&) = delete;

        /**
         * Adds a static property to this type.
         *
         * @tparam V The property value type.
         * @param name The property name.
         * @param type The property type.
         * @param access Functions which control
         */
        template <typename V>
        void Add(const Key& name, Type<V>* type, const PropertyCreate<T, V> access);

        /**
         * Gets static the property on this type with the given name
         *
         * @param name The name of the desired property.
         * @param requiredType The type to make sure is being returned, if any.
         * @return A pointer to the property or nullptr if it does not exist or match the required type.
         */
        Property* Get(const Key& name, TypeBase* requiredType = nullptr);

        /**
         * Gets the dynamic property on this type with the given name.
         *
         * @param name The name of the desired property.
         * @param requiredType The type to make sure is being returned, if any.
         * @return A pointer to the property or nullptr if it does not exist or match the required type.
         */
        Property* GetDynamic(const Key& name, TypeBase* requiredType = nullptr);

        /**
         * Returns the factory that produces dynamic properties.
         */
        inline PropertyFactory<T> GetFactory() const { return m_factory; }

        /**
         * Sets the factory that produces dynamic properties.
         *
         * @param factory The dynamic property factory, or `{}` to disable dynamic properties.
         */
        void SetFactory(PropertyFactory<T> factory) { m_factory = factory; }

        /**
         * Gets the map of static properties defined on this type.
         */
        std::map<Key, Property*> GetPropertyMap() const { return m_props; }

        /**
         * Sets the map of static properties defined on this type.
         */
        void SetPropertyMap(const std::map<Key, Property*>& props) { m_props = props; }

        /**
         * Returns the list of properties that exist on this type. An instance can be given to also return the dynamic
         * properties available for that instance.
         *
         * @param instance The instance to return dynamic properties for, if any.
         * @return A list of available properties.
         */
        std::vector<Key> GetPropertyNames(T* instance = nullptr, MatchCondition matches = Match::Any()) const;

        /**
         * Builds an Access which can get & set values of type V in the given property chain. If the property chain does
         * not exist then a falsy struct is returned. The access returned will have get & set functions which will be truthy
         * based on whether the property can be read or written. If the property chain isn't comprised entirely of
         * properties that support the ref function then temporary values will be allocated while values are being gotten
         * or set. This should be cached for the lifetime of the object you need access to if you plan on accessing it
         * very frequently.
         *
         * @tparam V The value type.
         * @param props The property chain.
         * @param setNulls If we should allow setting nulls. Some property types interpret nullptr as a removal of the property.
         * @return A struct which supplies access.
         */
        template <typename V = void>
        Access<T, V> GetAccess(const std::vector<Key> props, MatchCondition allMatch = Match::Any(), bool setNulls = false);

    protected:
        std::map<Key, Property*> m_props;
        PropertyFactory<T> m_factory;
    };

    /**
     * A way for classes to expose their non-public properties via friend.
     *
     * @see Type
     * @tparam T
     */
    template <typename T>
    void InitType(Type<T>*) { }


    /** Type Implementation */

    template <typename T>
    template <typename V>
    void Type<T>::Add(const Key& name, Type<V>* type, const PropertyCreate<T, V> access)
    {
        m_props[name] = Property::Create<T, V>(name, this, type, access.ref, access.get, access.set, access.flags);
    }

    template <typename T>
    Property* Type<T>::Get(const Key& name, TypeBase* requiredType)
    {
        auto prop = m_props[name];

        if (prop && requiredType != nullptr && prop->type != requiredType)
        {
            prop = nullptr;
        }

        return prop;
    }

    template <typename T>
    Property* Type<T>::GetDynamic(const Key& name, TypeBase* requiredType)
    {
        if (!m_factory.get)
        {
            return nullptr;
        }

        auto prop = m_factory.get(name, requiredType);

        if (prop && requiredType && prop->type != requiredType)
        {
            delete prop;

            prop = nullptr;
        }

        return prop;
    }

    template <typename T>
    std::vector<Key> Type<T>::GetPropertyNames(T* instance, MatchCondition matches) const
    {
        auto names = instance != nullptr && m_factory.available
            ? m_factory.available(instance, matches)
            : std::vector<Key>();

        for (auto& pair : m_props)
        {
            if (matches(pair.second->flags))
            {
                names.push_back(pair.first);
            }
        }

        return names;
    }

    template <typename T>
    template <typename V>
    Access<T, V> Type<T>::GetAccess(const std::vector<Key> props, MatchCondition allMatch, bool setNulls)
    {
        Access<T, V> out = {
            .keys = KeyList(props), 
        };

        // If no properties were passed, they are looking to manipulate the value they're passing in directly.
        if (props.empty())
        {
            out.get = [](T* instance) -> AccessValue<V>
            {
                return { (V*)instance };
            };

            out.set = [](T* instance, V* value) -> bool
            {
                if (value == nullptr || instance == nullptr)
                {
                    return false;
                }

                *(V*)instance = *value;

                return true;
            };

            return out;
        }

        // Build the chain of properties we need to traverse from type T to the desired V.
        TypeBase* current = this;
        bool canGet = true;
        bool canSet = true;

        for (auto& prop : props)
        {
            auto currentType = (Type<void>*)current;
            auto property = currentType->Get(prop);

            if (property == nullptr)
            {
                // Static property does not exist, try dynamic
                property = currentType->GetDynamic(prop);

                if (property != nullptr)
                {
                    // Keep track of dynamic so its freed once this access is freed.
                    out.dynamic.push_back(std::shared_ptr<Property>(property));
                }
                else
                {
                    return out;
                }
            }

            // If this property in the chain doesn't match the required values then we can't get access.
            if (!allMatch(property->flags))
            {
                return out;
            }

            // Cache list of properties so access get & set can quickly iterate over them.
            out.properties.push_back(property);

            current = property->type;

            if (current == nullptr)
            {
                return out;
            }

            // Only bother setting the get if all the properties have either get or ref.
            if (!property->ref && !property->get)
            {
                canGet = false;
            }

            // Only bother setting the set if all the properties have either ref or set.
            if (!property->ref && !property->set)
            {
                canSet = false;
            }
        }

        if (canGet)
        {
            out.get = [&out](T* instance) -> AccessValue<V>
            {
                if (instance == nullptr)
                {
                    return nullptr;
                }

                std::vector<std::shared_ptr<void>> allocated;
                void* result = (void*)instance;

                for (auto property : out.properties)
                {
                    if (property->ref)
                    {
                        result = property->ref(result);
                    }
                    else if (property->get)
                    {
                        auto next = property->type->Create();
                        auto gotten = property->get(result, next.get());
                        allocated.push_back(next);
                        result = gotten ? next.get() : nullptr;
                    }
                    else
                    {
                        result = nullptr;
                    }

                    if (result == nullptr)
                    {
                        break;
                    }
                }

                return { (V*)result, allocated };
            };
        }

        if (canSet)
        {
            out.set = [&out, setNulls](T* instance, V* value) -> bool
            {
                if (instance == nullptr || (!setNulls && value == nullptr))
                {
                    return false;
                }

                std::vector<std::shared_ptr<void>> allocated;
                std::vector<void*> values{};
                void* result = (void*)instance;

                values.push_back(instance);

                for (auto property : out.properties)
                {
                    if (property->ref)
                    {
                        result = property->ref(result);
                    }
                    else if (property->get)
                    {
                        auto next = property->type->Create();
                        auto gotten = property->get(result, next.get());
                        allocated.push_back(next);
                        result = gotten ? next.get() : nullptr;
                    }
                    else
                    {
                        result = nullptr;
                    }

                    if (result == nullptr)
                    {
                        return false;
                    }

                    values.push_back(result);
                }

                if (result == nullptr)
                {
                    return false;
                }

                // If a value was given, apply it to the last referenced value.
                if (value != nullptr)
                {
                    *((V*)result) = *value;
                }

                // If we have no temporary values, our job is done here.
                if (allocated.empty())
                {
                    return true;
                }

                // There are temporary values because not all properties support ref. If set exists we need to copy
                // the temporary value back to the property, starting at the end of the property chain and moving up.
                for (int i = out.properties.size() - 1; i >= 0; i--)
                {
                    auto property = out.properties.at(i);

                    if (property->set)
                    {
                        auto& instance = values.at(i);
                        auto& instanceValue = values.at(i + 1);

                        property->set(instance, instanceValue);
                    }
                }

                return true;
            };
        }

        return out;
    }

}