#include "gtest/gtest.h"

#include "graphics2/diligent/pass/PassManifest.h"
#include "graphics2/diligent/pass/PassTypes.h"

#include "ncengine/graphics/GraphicsUtility.h"
#include "ncengine/graphics/Material.h"
#include "ncengine/graphics/PostProcess.h"

#include "ncutility/NcError.h"

#include <span>
#include <vector>

using namespace nc;
using namespace nc::graphics;

TEST(PassManifestTest, EmptyPassDescs_SinkCountsEqualOne)
{
    /* If there are no passes, we still need to make our final pass that renders nothing to the swapchain. */
    auto passDescs = std::vector<PassDesc>{};

    auto uut = PassManifest(passDescs,  std::span<const MaterialPassFlag::type>{}, std::span<const PostProcessPassFlag::type>{}, std::span<const MiscPassFlag::type>{});

    EXPECT_EQ(uut.ColorSinkCount(), 0);
    EXPECT_EQ(uut.DepthSinkCount(), 0);
}

TEST(PassManifestTest, DuplicatePassID_Throws)
{
    /* For manifest generation purposes, IDs are generated from hashing the vertex and pixel shader paths with the name. */
    auto passDescs = std::vector<PassDesc>{
        PassDesc{
            .id = MaterialPassFlag::Toon,
            .name = "Toon",
            .type = PassType::Material,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = MainColor
        },
        PassDesc{
            .id = MaterialPassFlag::Normals,
            .name = "Toon",
            .type = PassType::Material,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = MainColor,
            .depthSink = MainDepth
        },};

    auto materialPassFlags = std::vector<MaterialPassFlag::type>{MaterialPassFlag::Toon, MaterialPassFlag::Normals};

    EXPECT_THROW(PassManifest(passDescs, materialPassFlags, std::span<const PostProcessPassFlag::type>{}, std::span<const MiscPassFlag::type>{}), NcError);
}

TEST(PassManifestTest, DifferentPassNameSameShader_AddedToManifest)
{
    /* For manifest generation purposes, IDs are generated from hashing the vertex and pixel shader paths with the name. */
    auto passDescs = std::vector<PassDesc>{
        PassDesc{
            .id = MaterialPassFlag::Toon,
            .name = "Toon",
            .type = PassType::Material,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = MainColor,
            .depthSink = MainDepth
        },
        PassDesc{
            .id = MaterialPassFlag::Normals,
            .name = "ToonSkinned",
            .type = PassType::SkinnedMaterial,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = MainColor,
            .depthSink = MainDepth
        },};

    auto materialPassFlags = std::vector<MaterialPassFlag::type>{MaterialPassFlag::Toon, MaterialPassFlag::Normals};

    auto uut = PassManifest(passDescs, materialPassFlags, std::span<const PostProcessPassFlag::type>{}, std::span<const MiscPassFlag::type>{});
    EXPECT_EQ(uut.ColorSinkCount(), 1);
    EXPECT_EQ(uut.DepthSinkCount(), 2); // MainDepth is set to be index 1 instead of zero to avoid collision with MainDepthMsaa
}

TEST(PassManifestTest, PassesRenderOnlyToSwapchain_SinkCountsAreZero)
{
    /* ColorSinkCount and DepthSinkCount are used to create the offscreen sink targets. No need to create the swapchain targets. */
    auto passDescs = std::vector<PassDesc>{
        PassDesc{
            .id = MaterialPassFlag::Toon,
            .name = "Toon",
            .type = PassType::Material,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = SwapChainColorRTIndex,
            .depthSink = SwapChainDepthRTIndex
        },
        PassDesc{
            .id = MaterialPassFlag::Normals,
            .name = "ToonSkinned",
            .type = PassType::SkinnedMaterial,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = SwapChainColorRTIndex,
            .depthSink = SwapChainDepthRTIndex
        },};

    auto materialPassFlags = std::vector<MaterialPassFlag::type>{MaterialPassFlag::Toon, MaterialPassFlag::Normals};

    auto uut = PassManifest(passDescs, materialPassFlags, std::span<const PostProcessPassFlag::type>{}, std::span<const MiscPassFlag::type>{});
    EXPECT_EQ(uut.ColorSinkCount(), 0);
    EXPECT_EQ(uut.DepthSinkCount(), 0);
}

TEST(PassManifestTest, PassIDNotPresentInPassFlags_PassNotAddedToManifest)
{
    auto passDescs = std::vector<PassDesc>
    {
        PassDesc
        {
            .id = MaterialPassFlag::Toon,
            .name = "Toon",
            .type = PassType::Material,
            .shaderPaths = ShaderPaths{"Toon.psh", "Toon.vsh"},
            .colorSink = SwapChainColorRTIndex,
            .depthSink = SwapChainDepthRTIndex
        }
    };

    auto materialPassFlags = std::vector<MaterialPassFlag::type>{MaterialPassFlag::Normals};

    auto uut = PassManifest(passDescs, materialPassFlags, std::span<const PostProcessPassFlag::type>{}, std::span<const MiscPassFlag::type>{});
    auto staticPassDescs = uut.StaticMaterialPassDescs();

    EXPECT_TRUE(staticPassDescs.empty());
}
