#pragma once

#include <functional>


namespace nc
{

    using MatchCondition = std::function<bool(size_t)>;

    /** Used for querying based on bits/flags. */
    class Match
    {
    public:
        static MatchCondition And(MatchCondition a, MatchCondition b);
        static MatchCondition Or(MatchCondition a, MatchCondition b);
        static MatchCondition Not(MatchCondition a);
        static MatchCondition Any();
        static MatchCondition Nothing();
        static MatchCondition All(size_t x);
        static MatchCondition Some(size_t x);
        static MatchCondition None(size_t x);
        static MatchCondition NotAll(size_t x);
        static MatchCondition Exactly(size_t x);
    };

}