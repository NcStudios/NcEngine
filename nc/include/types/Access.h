#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "utility/Key.h"
#include "types/Property.h"


namespace nc
{

    /**
     * The result of accessing a property of type V embedded at any level within an instance. This also stores a list of
     * shared pointers of temporary values that had to be allocated to get the value. They will be released when this value
     * is deallocated. Getting a value can create temporary values when the property chain has at least one property that
     * doesn't support the ref function.
     *
     * @tparam V The type of the value returned.
     */
    template <typename V>
    class AccessValue
    {
    public:

        AccessValue(V* value, std::vector<std::shared_ptr<void>> allocated = {})
            : m_value(value),
              m_allocated(std::move(allocated)) {}

        AccessValue(std::nullptr_t)
            : m_value(nullptr),
              m_allocated() {}

        // If the value is a reference to the exact value in the property chain. That means it can be updated directly.
        // with the assignment operator.
        bool ref() const { return m_value != nullptr && m_allocated.empty(); }

        // If the value could be accessed.
        bool exists() const { return m_value != nullptr; }

        // The raw value. If this value does not exist this will cause an error.
        V get() { return *m_value; }

        // Gets the raw value, and if it doesn't exist returns the given value instead.
        V get(V otherwise) { return m_value != nullptr ? *m_value : otherwise; }

        // Can be checked for existence of a value in a boolean condition.
        operator bool() const { return exists(); }

        // Provides access to the internal value type. If the value does not exist this will cause an error.
        V* operator ->() const { return m_value; }

        // When cast to a pointer of type V
        operator const V* () const { return m_value; }

        // Safely set the value.
        void operator = (const V& newValue)
        {
            if (m_value != nullptr)
            {
                *m_value = newValue;
            }
        }
    private:
        // The reference to the value returned. If it couldn't be gotten this will be a nullptr.
        V* m_value;

        // The list of temporary values that had to be allocated to compute the final value.
        const std::vector<std::shared_ptr<void>> m_allocated;
    };

    /**
     * A struct which provides access to a property of type V in an instance of type T in a given property chain.
     *
     * @tparam T The instance type.
     * @tparam V The value type.
     */
    template <typename T = void, typename V = void>
    struct Access
    {
        // The keys of the property list. Used as a key for an Access map.
        KeyList keys = {};

        // The property chain to get from T to V.
        std::vector<Property*> properties = {};

        // The dynamic properties that had to be created and will be freed once this Access is no longer used.
        std::vector<std::shared_ptr<Property>> dynamic = {};

        // The function which can get a value from an instance. This will be falsy if get is not supported.
        std::function<AccessValue<V>(T*)> get = {};

        // The function which can set a value on an instance. This will be falsy if set is not supported.
        std::function<bool(T*, V*)> set = {};

        // Returns whether this Access can get or set values on an instance. If the property chain is valid this will be true.
        inline operator bool() const { return (bool)get && (bool)set; }

        // Safely call get by invoking Access like a function passing an instance. `value = access(instance)`
        AccessValue<V> operator ()(T* instance) const { return get ? get(instance) : nullptr; }
        AccessValue<V> operator ()(T& instance) const { return get ? get(&instance) : nullptr; }

        // Safely call set by invoking Access like a function passing an instance and value. `bool set = access(instance, value)`
        bool operator ()(T* instance, V* value) const { return set ? set(instance, value) : false; }
        bool operator ()(T* instance, V value) const { return set ? set(instance, &value) : false; }
        bool operator ()(T* instance, V& value) const { return set ? set(instance, &value) : false; }
        bool operator ()(T& instance, V* value) const { return set ? set(&instance, value) : false; }
        bool operator ()(T& instance, V value) const { return set ? set(&instance, &value) : false; }
        bool operator ()(T& instance, V& value) const { return set ? set(&instance, &value) : false; }
    };

}