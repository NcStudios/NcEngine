#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/Dearchiver.h"

#include "Graphics/GraphicsEngine/interface/Dearchiver.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#ifndef NC_DEARCHIVER_TEST_COLLATERAL_DIR
#error "NC_DEARCHIVER_TEST_COLLATERAL_DIR not defined"
#endif

constexpr auto g_archive = NC_DEARCHIVER_TEST_COLLATERAL_DIR"/archive.bin";
constexpr auto g_corruptArchive = NC_DEARCHIVER_TEST_COLLATERAL_DIR"/corrupt_archive.bin";
constexpr auto g_resourceSignatureKey = "TestSignature";
constexpr auto g_resourceVariableKey = "Constants";
constexpr auto g_psoKey = "TestPSO";

class DearchiverTest : public DiligentEngineParameterizedFixture
{
    protected:
        std::unique_ptr<nc::graphics::Dearchiver> uut;

        DearchiverTest()
            : DiligentEngineParameterizedFixture{false}
        {
        }

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            uut = std::make_unique<nc::graphics::Dearchiver>(
                engine->GetDevice(),
                engine->GetSwapChain(),
                engine->GetEngineFactory()
            );
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, DearchiverTest, g_apiParams);

TEST_P(DearchiverTest, HappyPath)
{
    ASSERT_NO_THROW(uut->Load(g_archive));

    auto actualSignature = Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature>{};
    auto actualPSO = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    ASSERT_NO_THROW(actualSignature = uut->UnpackResourceSignature(g_resourceSignatureKey));
    ASSERT_NO_THROW(actualPSO = uut->UnpackGraphicsPipelineState(g_psoKey));

    ASSERT_EQ(1, actualPSO->GetResourceSignatureCount());
    EXPECT_EQ(actualSignature.RawPtr(), actualPSO->GetResourceSignature(0));

    auto srb = Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>{};
    actualSignature->CreateShaderResourceBinding(&srb);
    ASSERT_NE(nullptr, actualSignature);;
    ASSERT_NE(nullptr, srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, g_resourceVariableKey));

    ASSERT_NO_THROW(uut->Clear());

    auto& context = engine->GetContext();
    context.CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    context.SetPipelineState(actualPSO);
}

TEST_P(DearchiverTest, FailurePaths)
{
    EXPECT_THROW(uut->Load("does_not_exist.bin"), nc::NcError);
    EXPECT_THROW(uut->Load(g_corruptArchive), nc::NcError);

    ASSERT_NO_THROW(uut->Load(g_archive));
    EXPECT_THROW(uut->UnpackResourceSignature("InvalidSignature"), nc::NcError);
    EXPECT_THROW(uut->UnpackGraphicsPipelineState("InvalidPSO"), nc::NcError);

    ClearErrorOutput();
}
