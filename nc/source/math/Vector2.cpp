#include "math/Vector2.h"

#include "imgui/imgui.h"

namespace nc
{
    Vector2::Vector2(const ImVec2& vec) noexcept
        : x(vec.x), y(vec.y)
    {
    }
    
    Vector2::Vector2(ImVec2&& vec) noexcept
        : x(vec.x), y(vec.y)
    {
    }
    
    Vector2& Vector2::operator=(const ImVec2& vec) noexcept
    {
        x = vec.x; y = vec.y;
        return *this;
    }
    
    Vector2& Vector2::operator=(ImVec2&& vec) noexcept
    {
        x = vec.x; y = vec.y;
        return *this;
    }

    ImVec2 Vector2::ToImVec2() noexcept
    {
        return ImVec2{x,y};
    }
}