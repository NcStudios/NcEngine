#pragma once

struct ResultCode
{
    static constexpr auto Success = 0;
    static constexpr auto RuntimeError = 1;
    static constexpr auto ArgumentError = 2;
};
