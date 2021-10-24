#pragma once

#include <functional>

#include "utility/Key.h"
#include "utility/Match.h"


namespace nc
{

    class TypeBase;

    /**
     * System flags placed on properties for
     */
    enum PropertyFlags
    {
        // A property can be read by default. This flag is automatically assigned.
        NoRead        = (1 << 0),
        // A property can be written to by default. This flag is automatically assigned.
        NoWrite       = (1 << 1),
        // A property can be animated by default (assuming it has a calculator). You must explicitly say it should not be available to animate.
        NoAnimate     = (1 << 2),
        // A property can be displayed to debugging output by default, pass this to disable it.
        NoDebug       = (1 << 3),
    };

    /**
     * If a property supports it, this will return the reference to the property value of type V for an instance of T.
     * Specifying this function will make the property readable & writable.
     */
    template <typename T, typename V>
    using PropertyRef = std::function<V*(T* instance)>;

    /**
     * If a property supports it, this will copy the value of type V to out which will be a non-null pointer.
     * Specifying this function will make the property readable.
     */
    template <typename T, typename V>
    using PropertyGet = std::function<bool(T* instance, V* out)>;

    /**
     * If a property supports it, this will copy the value and place it in the property of an instance of T.
     * Specifing this function will make the property writable.
     */
    template <typename T, typename V>
    using PropertySet = std::function<void(T* instance, V* value)>;


    /**
     * The functions which provide access to the values of type V in an instance of type T. Not specifying one of these
     * or giving them a value of `{}` will disable that function. You can achieve varying access in the following way:
     *
     * - If you want a read only property specify `get` only.
     * - If you want a write only property specify `set` only.
     * - If you want a read-write property specify `ref` if possible, otherwise `set` and `get.
     *
     * @tparam T The instance type.
     * @tparam V The value type.
     */
    template <typename T, typename V>
    struct PropertyCreate
    {
        PropertyRef<T, V> ref = {};
        PropertyGet<T, V> get = {};
        PropertySet<T, V> set = {};
        Flags flags = 0;
    };

    /**
     * A property on a type. It has a name, reference to the parent type, a reference to the property type, and
     * functions which can possibly set & get values to & from the property on an instance of the parent type.
     */
    struct Property
    {
        const Key name;
        TypeBase* parent;
        TypeBase* type;
        PropertyRef<void, void> ref;
        PropertyGet<void, void> get;
        PropertySet<void, void> set;
        Flags flags;

        Property(const Key& _name, TypeBase* _parent, TypeBase* _type, PropertyRef<void, void> _ref, PropertyGet<void, void> _get, PropertySet<void, void> _set, size_t _flags )
            : name(_name), 
              parent(_parent), 
              type(_type),
              ref(_ref), 
              get(_get), 
              set(_set),
              flags((get || ref ? 0 : PropertyFlags::NoRead) | (set || ref ? 0 : PropertyFlags::NoWrite) | _flags) {}
        
        Property(const Property&) = delete;
        ~Property() = default;
        Property& operator=(const Property& rhs) = delete;

        /**
         * Creates a type safe property for instances of type T and values of type V.
         *
         * @tparam T The instance type.
         * @tparam V The value type.
         * @param name The property name.
         * @param parent The parent type.
         * @param type THe property type.
         * @param ref The function to get a reference to the property value. To disable pass `{}`.
         * @param get The function to get a copy of the property value. To disable pass `{}`.
         * @param set The function to set the value of the property. To disable pass `{}`.
         * @return A new Property.
         */
        template <typename T, typename V>
        static Property* Create(const Key& name, TypeBase* parent, TypeBase* type, PropertyRef<T, V> ref, PropertyGet<T, V> get, PropertySet<T, V> set, size_t flags) {
            // TODO move this somewhere else

            return new Property(
                name,
                parent,
                type,
                *((PropertyRef<void, void>*)&ref),
                *((PropertyGet<void, void>*)&get),
                *((PropertySet<void, void>*)&set),
                flags
            );
        }
    };

    /**
     * A factory which can provide properties for a type at runtime based on the specific instance.
     *
     * @tparam T The instance type.
     */
    template <typename T>
    struct PropertyFactory
    {
        using get_type = std::function<Property*(Key name, TypeBase* requiredType)>;
        using available_type = std::function<std::vector<Key>(T* instance, const MatchCondition& matches)>;

        // Given a property name and possibly a required type, return a new Property or nullptr if it doesn't exist.
        get_type get;

        // Given an instance return the list of valid dynamic properties we can get.
        available_type available;

        // Determines whether this factory produces dynamic properties.
        inline operator bool() const { return (bool)get; }
    };

}