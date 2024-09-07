#pragma once

#include "ncengine/ecs/Component.h"

#include "nccommon/ncmath/Vector.h"


#include <string_view>

namespace nc::graphics
{
class MaterialInstance : public ComponentBase
{
    void SetFloat(std::string_view resName, float value);
    void SetFloat(std::string_view resName, float value, std::string_view displayName);

    void SetUInt32(std::string_view resName, uint32_t value);
    void SetUInt32(std::string_view resName, uint32_t value, std::string_view displayName);

    void SetInt32(std::string_view resName, int32_t value);
    void SetInt32(std::string_view resName, int32_t value, std::string_view displayName);

    void SetTexture(std::string_view resName, std::string_view value);
    void SetTexture(std::string_view resName, std::string_view value, std::string_view displayName);

    void SetBoolean(std::string_view resName, bool value);
    void SetBoolean(std::string_view resName, bool value, std::string_view displayName);

    void SetVector2(std::string_view resName, const nc::Vector2& value);
    void SetVector2(std::string_view resName, const nc::Vector2& value, std::string_view displayName);

    void SetVector3(std::string_view resName, const nc::Vector3& value);
    void SetVector3(std::string_view resName, const nc::Vector3& value, std::string_view displayName);

    void SetVector4(std::string_view resName, const nc::Vector4& value);
    void SetVector4(std::string_view resName, const nc::Vector4& value, std::string_view displayName);

    void SetMatrix4x4(std::string_view resName, DirectX::FXMMATRIX value);
    void SetMatrix4x4(std::string_view resName, DirectX::FXMMATRIX value, std::string_view displayName);

    const auto GetFloat(std::string_view resName) -> float;
    const auto GetUInt32(std::string_view resName) -> uint32_t;
    const auto GetInt32(std::string_view resName) -> int32_t;
    const auto GetTexture(std::string_view resName) -> std::string_view; /** Return texture view? */
    const auto GetVector2(std::string_view resName) -> const nc::Vector2&;
    const auto GetVector3(std::string_view resName) -> const nc::Vector3&;
    const auto GetVector4(std::string_view resName) -> const nc::Vector4&;
    const auto GetMatrix4x4(std::string_view resName) -> DirectX::FXMMATRIX;
}
} //namespace nc::graphics
