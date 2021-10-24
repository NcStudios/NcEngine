#pragma once
#include "debug/Utils.h"


namespace nc
{

    using KeyValue = size_t;

    /** An improved map key for strings. */
    struct Key 
    {
        KeyValue value;
        char* text;

        constexpr Key()                  noexcept : value(0), text(nullptr) {}
        constexpr Key(const char* input) noexcept; // : value(hash_const(input)), text(input) {}
        Key(const std::string input)     noexcept; // : value(hash_live(input)), text(input.c_str()) {}

        operator size_t()      const noexcept { return value; }
        operator const char*() const noexcept { return text; }
        operator bool()        const noexcept { return text != nullptr; }

        constexpr Key& operator =(const Key& rhs) noexcept
        {
            value = rhs.value;
            text = rhs.text;
            return *this;
        }

        // static KeyValue constexpr hash_const(char const *input) noexcept;
        // static KeyValue hash_live(const std::string input) noexcept;
    };

    constexpr auto KeyListMazSize = 4;

    /** A list of keys */
    struct KeyList 
    {
        KeyValue value;
        Key keys[KeyListMazSize]; // Store at most 4 keys, that should be enough

        constexpr KeyList(const std::initializer_list<Key> input) noexcept;
        template <typename Iter> KeyList(const Iter iterable)     noexcept;

        constexpr size_t Size() const noexcept;

        operator size_t() const noexcept { return value; }
        operator bool()   const noexcept { return Size() != 0; }
    };

    /* Key & KeyList Functions */
    constexpr auto operator ==(const Key& lhs, const Key& rhs) -> bool;
    constexpr auto operator !=(const Key& lhs, const Key& rhs) -> bool;
    constexpr auto operator <(const Key& lhs, const Key& rhs) noexcept -> bool;
    constexpr auto operator <=(const Key& lhs, const Key& rhs) noexcept -> bool;
    constexpr auto operator >(const Key& lhs, const Key& rhs) noexcept -> bool;
    constexpr auto operator >=(const Key& lhs, const Key& rhs) noexcept -> bool;

    constexpr auto operator ==(const KeyList& lhs, const KeyList& rhs) -> bool;
    constexpr auto operator !=(const KeyList& lhs, const KeyList& rhs) -> bool;
    constexpr auto operator <(const KeyList& lhs, const KeyList& rhs) noexcept -> bool;
    constexpr auto operator <=(const KeyList& lhs, const KeyList& rhs) noexcept -> bool;
    constexpr auto operator >(const KeyList& lhs, const KeyList& rhs) noexcept -> bool;
    constexpr auto operator >=(const KeyList& lhs, const KeyList& rhs) noexcept -> bool;
    
    /* Key Implementation */
    constexpr bool operator ==(const Key& lhs, const Key& rhs)
    {
        IF_THROW(lhs.value == rhs.value && std::string(lhs.text) != std::string(rhs.text), "Keys have same value but different text: " + std::string(lhs.text) + " and " + std::string(rhs.text));

        return lhs.value == rhs.value;
    }

    constexpr bool operator !=(const Key& lhs, const Key& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr bool operator <(const Key& lhs, const Key& rhs) noexcept 
    {
        return lhs.value < rhs.value;
    }

    constexpr bool operator <=(const Key& lhs, const Key& rhs) noexcept 
    {
        return lhs.value <= rhs.value;
    }

    constexpr bool operator >(const Key& lhs, const Key& rhs) noexcept 
    {
        return lhs.value > rhs.value;
    }

    constexpr bool operator >=(const Key& lhs, const Key& rhs) noexcept 
    {
        return lhs.value >= rhs.value;
    }

    constexpr KeyValue hash_input(const char* input)
    {
        char* c = (char*)input;
        KeyValue hash = 5381;

        while (*c)
        {
            hash *= 33;
            hash += static_cast<KeyValue>(*c);
            c++;
        }

        return hash;
    }

    constexpr Key::Key(const char* input) noexcept
    {
        text = (char*)input;
        value = hash_input(text);
    }

    Key::Key(const std::string input) noexcept
    {
        text = (char*)input.c_str();
        value = hash_input(text);
    }

    /* KeyList Implementation */
    
    constexpr bool operator ==(const KeyList& lhs, const KeyList& rhs)
    {
        return lhs.value == rhs.value;
    }

    constexpr bool operator !=(const KeyList& lhs, const KeyList& rhs)
    {
        return !(lhs == rhs);
    }

    constexpr bool operator <(const KeyList& lhs, const KeyList& rhs) noexcept 
    {
        return lhs.value < rhs.value;
    }

    constexpr bool operator <=(const KeyList& lhs, const KeyList& rhs) noexcept 
    {
        return lhs.value <= rhs.value;
    }

    constexpr bool operator >(const KeyList& lhs, const KeyList& rhs) noexcept 
    {
        return lhs.value > rhs.value;
    }

    constexpr bool operator >=(const KeyList& lhs, const KeyList& rhs) noexcept 
    {
        return lhs.value >= rhs.value;
    }

    constexpr KeyValue hash_keys(const std::initializer_list<Key> keys)
    {
        KeyValue hash = 29;

        for (auto& key : keys)
        {
            hash *= 33;
            hash += key.value;
        }

        return hash;
    }

    constexpr void build_keys(const std::initializer_list<Key> keys, Key out[4])
    {
        size_t i = 0;

        for (auto& key : keys)
        {
            out[i++] = key;

            if (i == KeyListMazSize)
            {
                return;
            }
        }
    }

    constexpr KeyList::KeyList(const std::initializer_list<Key> input) noexcept
    {
        value = hash_keys(input);
        build_keys(input, keys);
    }

    template <typename Iter> 
    KeyList::KeyList(const Iter iterable) noexcept
    {
        KeyValue hash = 29;
        size_t i = 0;

        for (Key& key : iterable)
        {
            hash *= 33;
            hash += key.value;

            if (i < KeyListMazSize)
            {
                keys[i++] = key;
            }
        }

        value = hash;
    }

    constexpr size_t KeyList::Size() const noexcept
    {
        for (auto i = 0; i < KeyListMazSize; i++) 
        {
            if (!static_cast<bool>(keys[i])) 
            {
                return i;
            }
        }

        return KeyListMazSize;
    }

}
