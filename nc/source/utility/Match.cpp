
#include "utility/Match.h"


namespace nc
{
    /** Match Implementation */

    MatchCondition Match::And(MatchCondition a, MatchCondition b)
    {
        return [a = std::move(a), b = std::move(b)](size_t n) { return a(n) && b(n); };
    }

    MatchCondition Match::Or(MatchCondition a, MatchCondition b) 
    {
        return [a = std::move(a), b = std::move(b)](size_t n) { return a(n) || b(n); };
    }

    MatchCondition Match::Not(MatchCondition a) 
    {
        return [a = std::move(a)](size_t n) { return !a(n); };
    }

    MatchCondition Match::Any() 
    {
        return [](size_t) { return true; };
    }

    MatchCondition Match::Nothing() 
    {
        return [](size_t) { return false; };
    }

    MatchCondition Match::All(size_t x) 
    {
        return [x](size_t n) { return (n & x) == x; };
    }

    MatchCondition Match::Some(size_t x) 
    {
        return [x](size_t n) { return (n & x) != 0; };
    }

    MatchCondition Match::None(size_t x) 
    {
        return [x](size_t n) { return (n & x) == 0; };
    }

    MatchCondition Match::NotAll(size_t x) 
    {
        return [x](size_t n) { return (n & x) != x; };
    }

    MatchCondition Match::Exactly(size_t x) 
    {
        return [x](size_t n) { return n == x; };
    }
}