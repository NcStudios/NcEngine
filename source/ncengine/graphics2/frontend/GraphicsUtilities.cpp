#include "GraphicsUtilities.h"

namespace nc::graphics
{
auto TransposeIfRequired(const DirectX::XMMATRIX& modelMatrix, bool ) -> DirectX::XMMATRIX
{
    return modelMatrix;
}
}
