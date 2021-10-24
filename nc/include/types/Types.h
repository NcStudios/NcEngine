#pragma once

#include <map>
#include <vector>
#include <functional>
#include <string>

#include "utility/Key.h"
#include "utility/Match.h"
#include "types/Type.h"
#include "types/Property.h"


namespace nc
{

    /**
     * A registry of all defined types.
     */
    class Types
    {
        template <typename K>
        using name_to_key = std::function<K(Key)>;

        template <typename K>
        using key_to_name = std::function<K(Key)>;

        template <typename V>
        using value_creator = std::function<V()>;

        template <typename T>
        using inline_init = std::function<void(Type<T>*)>;
    public:

        /**
         * Gets the Type with the given name or returns nullptr if it does not exist.
         *
         * @param name The name of the type.
         * @return The reference if it exists, otherwise nullptr.
         */
        static TypeBase* Get(const Key& name);

        /**
         * Gets the type with the given name and value type. If it doesn't exist or appears to have the underlying type
         * a nullptr will be returned.
         *
         * @tparam T The instance type.
         * @param name The name of the type.
         * @return The reference if it exists, otherwise nullptr.
         */
        template <typename T>
        static Type<T>* Get(const Key& name);

        /**
         * Returns all defined types which has flags which matches the given condition.
         *
         * @param match The condition to use to evaluate each types flags.
         * @return A vector of matching types.
         */
        static std::vector<TypeBase*> Matches(MatchCondition match);

        /**
         * Defines a new type for the given type and name.
         *
         * @tparam T The type type.
         * @param name The name of the type.
         * @param init A function to call with the type to inline init the Type.
         * @return The new type.
         */
        template <typename T>
        static Type<T>* Define(
            const Key& name, 
            std::function<std::shared_ptr<void>()> create,
            inline_init<T> init = {}
        );

        
        /**
         * Defines a new type for the given type and name.
         *
         * @tparam T The type type.
         * @param name The name of the type.
         * @param init A function to call with the type to inline init the Type.
         * @return The new type.
         */
        template <typename T>
        static Type<T>* Define(
            const Key& name, 
            inline_init<T> init = {}
        );

        /**
         * Aliases an existing type with a new name. This allows specialization for anything that's tied to types. For
         * example, you could have a Type<float> but you want a variation that represents a percent, angles, etc.
         *
         * @tparam T The type type.
         * @param baseType The base type to alias.
         * @param name The name of the type.
         * @return The new type.
         */
        template <typename T>
        static Type<T>* Alias(Type<T>* baseType, const Key& name);

        /**
         * Extends an existing type with a new name. This is for types which inherit another type - this it has all the
         * same properties.
         *
         * @tparam T The type type.
         * @param baseType The base type to extend.
         * @param name The name of the type.
         * @param init A function to call with the type to inline init the Type.
         * @return The new type.
         */
        template <typename T, typename B> 
            requires std::is_base_of<B, T>::value
        static Type<T>* Extend(Type<B>* baseType, const Key& name, inline_init<T> init = {});

        /**
         * Creates a type for a map. By default the map key is a string, but with supplying functions to convert between
         * property names and keys you can use any map key that can be stored in a string.
         *
         * @tparam V The value type.
         * @tparam K The key type.
         * @param name The name of the map type.
         * @param valueType The Type for the values.
         * @param getKey Converts a property name to a map key.
         * @param getName Converts a map key to a property name.
         * @param createMissing If specified this function will create missing values on get.
         * @return The new type.
         */
        template <typename V, typename K = std::string>
        static Type<std::map<K, V>>* DefineMap(
            const Key& name,
            Type<V>* valueType,
            size_t valueFlags = 0,
            const name_to_key<K>& getKey = [](auto n){ return n; },
            const key_to_name<K>& getName = [](auto n){ return n; },
            const value_creator<V>& createMissing = {}
        );

        /**
         * Creates a type for a vector.
         *
         * @tparam V The value type.
         * @param name The name of the vector type.
         * @param valueType The Type for the values.
         * @param getMissing If specified this function will create missing values at the requested index on get.
         * @param setMissing If specified this function will create missing values at the requested index on set.
         * @return The new type.
         */
        template <typename V>
        static Type<std::vector<V>>* DefineVector(
            const Key& name,
            Type<V>* valueType,
            size_t valueFlags = 0,
            const value_creator<V>& getMissing = {},
            const bool setMissing = false
        );

        /**
         * Gets map of defined types.
         */
        static std::map<Key, TypeBase*>& GetMap();

        /**
         * Gets list of defined types.
         */
        static std::vector<TypeBase*>& GetList();
    };


    /** Types Implementation */

    template <typename T>
    Type<T>* Types::Get(const Key& name)
    {
        return reinterpret_cast<Type<T>*>(GetMap()[name]);
    }
    
    template <typename T>
    Type<T>* Types::Define(const Key& name, inline_init<T> init)
    {
        return Define(name, []() { return std::make_shared<T>(); }, init);
    }
    
    template <typename T>
    Type<T>* Types::Define(const Key& name, std::function<std::shared_ptr<void>()> create, inline_init<T> init)
    {
        auto& list = GetList();
        auto& map = GetMap();
        auto id = list.size();
        auto t = new Type<T>(id, name, create);

        list.push_back(t);
        map[name] = t;

        InitType<T>(t);

        if (init)
        {
            init(t);
        }

        return t;
    }

    template <typename T>
    Type<T>* Types::Alias(Type<T>* baseType, const Key& name)
    {
        auto aliased = Define<T>(name);

        aliased->SetFactory(baseType->GetFactory());
        aliased->SetPropertyMap(baseType->GetPropertyMap());

        return aliased;
    }

    template <typename T, typename B> 
        requires std::is_base_of<B, T>::value
    Type<T>* Types::Extend(Type<B>* baseType, const Key& name, inline_init<T> init)
    {
        auto extended = Define<T>(name);

        auto baseFactory = baseType->GetFactory();
        auto extendedFactory = reinterpret_cast<PropertyFactory<T>*>(&baseFactory);

        if (extendedFactory != nullptr)
        {
            extended->SetFactory(*extendedFactory);
        }

        extended->SetPropertyMap(baseType->GetPropertyMap());

        if (init)
        {
            init(extended);
        }

        return extended;
    }

    template <typename V, typename K>
    Type<std::map<K, V>>* Types::DefineMap(
        const Key& name,
        Type<V>* valueType,
        size_t valueFlags,
        const name_to_key<K>& getKey,
        const key_to_name<K>& getName,
        const value_creator<V>& createMissing
    ) 
    {
        using map_type = std::map<K, V>;
        auto mapType = Define<map_type>(name);

        mapType->SetFactory({
            .get = [createMissing, getKey, valueType, valueFlags, mapType](Key name, TypeBase* requiredType) -> Property*
            {
                if (requiredType != nullptr && valueType != requiredType)
                {
                    return nullptr;
                }

                auto key = getKey(name);

                PropertyRef<map_type, V> ref = [key, createMissing](map_type* map) -> V*
                {
                    auto first = map->find(key);

                    if (first != map->end())
                    {
                        return &first->second;
                    }

                    if (createMissing)
                    {
                        map->insert({ key, createMissing() });

                        return &map->find(key)->second;
                    }

                    return nullptr;
                };

                PropertySet<map_type, V> set = [key](map_type* map, V* value) -> bool
                {
                    auto first = map->find(key);
                    auto found = first != map->end();

                    if (value == nullptr)
                    {
                        if (found)
                        {
                            map->erase(first);
                        }
                    }
                    else if (found)
                    {
                        first->second = *value;
                    }
                    else
                    {
                        map->insert({ key, *value });
                    }

                    return true;
                };

                return Property::Create<map_type, V>(name, mapType, valueType, ref, {}, set, valueFlags );
            },
            .available = [&getName](map_type* map, auto)
            {
                std::vector<Key> names{};

                if (getName)
                {
                    for (auto pair = map->begin(); pair != map->end(); pair++)
                    {
                        names.push_back(getName(pair->first));
                    }
                }

                return names;
            },
        });

        return mapType;
    }

    template <typename V>
    Type<std::vector<V>>* Types::DefineVector(
        const Key& name,
        Type<V>* valueType,
        size_t valueFlags,
        const value_creator<V>& getMissing,
        const bool setMissing
    ) 
    {
        using vector_type = std::vector<V>;
        auto vectorType = Define<vector_type>(name);

        vectorType->SetFactory({
            .get = [getMissing, setMissing, valueType, valueFlags, vectorType](Key name, TypeBase* requiredType) -> Property*
            {
                if (requiredType != nullptr && valueType != requiredType)
                {
                    return nullptr;
                }

                int parsedIndex = -1;

                try
                {
                    parsedIndex = std::stoi(name.text);
                }
                catch (...) { }

                if (parsedIndex < 0)
                {
                    return nullptr;
                }

                size_t index = parsedIndex;

                PropertyRef<vector_type, V> ref = [index, getMissing](vector_type* vector) -> V*
                {
                    if (vector->size() <= index)
                    {
                        if (!getMissing)
                        {
                            return nullptr;
                        }

                        vector->reserve(index + 1);
                        vector->at(index) = getMissing();
                    }

                    return &vector->at(index);
                };

                PropertySet<vector_type, V> set = [index, setMissing](vector_type* vector, V* value) -> bool
                {
                    if (vector->size() <= index)
                    {
                        if (!setMissing)
                        {
                            return false;
                        }

                        vector->resize(index + 1);
                    }

                    if (value == nullptr)
                    {
                        vector->erase(vector->begin() + index);
                    }
                    else
                    {
                        vector->at(index) = *value;
                    }

                    return true;
                };

                return Property::Create<vector_type, V>(name, vectorType, valueType, ref, {}, set, valueFlags );
            },
            .available = [](vector_type* vector, auto)
            {
                std::vector<Key> names{};

                const auto n = vector->size();

                for (size_t i = 0; i < n; i++)
                {
                    names.push_back(std::to_string(i));
                }

                return names;
            },
        });

        return vectorType;
    }


}