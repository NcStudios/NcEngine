#pragma once

#include "ncmath/Vector.h"

#include "DirectXMath.h"

#include <string>

namespace nc::graphics
{

#ifdef _MSC_VER
#pragma warning( disable : 4324 ) // Justification: We want to be explicit about alignment for the GPU buffer structs.
#endif
struct GlobalData
{
    alignas(16)DirectX::XMMATRIX cameraViewProjMat;
    alignas(16)Vector3 cameraWorldPos = Vector3(0.0f, 0.0f, 0.0f);
    alignas(16)Vector3 ambientColor = Vector3(0.992f, 0.909f, 0.737f);
    alignas(4)uint32_t skyboxTextureIndex = 0u;
};

constexpr auto sizeOfData = alignof(Vector3);
constexpr auto sizeOfViewProj = sizeof(GlobalData);
#ifdef _MSC_VER
#pragma warning( default  : 4324 )
#endif

} // namespace nc::graphics
