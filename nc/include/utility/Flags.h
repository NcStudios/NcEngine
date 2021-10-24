#pragma once

#include "utility/Match.h"


namespace nc
{
    class Flags
    {
    private:
        size_t value;
    public:
        Flags(size_t bits = 0): value(bits) {}

        bool Is(const MatchCondition& match) const { return match(value); }
        void Add(size_t bits) { value |= bits; }
        void Remove(size_t bits) { value &= ~bits; }
        void Intersect(size_t bits) { value &= bits; }
        void Set(size_t bits) { value = bits; }
        void Flip() { value = ~value; }
        void Clear() { value = 0; }

        operator size_t() const { return value; }
        void operator =(size_t bits) { value = bits; }
    };
}