#include "DiligentEngineFixture.inl"
#include "graphics2/diligent/MaterialPass.h"

// class MaterialPassSystemTests : public DiligentEngineParameterizedFixture
class MaterialPassSystemTests : public testing::Test
{
    protected:
        std::unique_ptr<nc::graphics::MaterialPassSystem> uut;

        void SetUp() override
        {
            // INITIALIZE_DILIGENT_FIXTURE;
            // todo: actually make pipelines here?
            auto passes = std::vector<nc::graphics::Pass>{
                nc::graphics::Pass{{}, nc::MaterialPass::Shadow},
                nc::graphics::Pass{{}, nc::MaterialPass::Toon},
                nc::graphics::Pass{{}, nc::MaterialPass::Alpha},
            };

            uut = std::make_unique<nc::graphics::MaterialPassSystem>(std::move(passes));
        }

        void TearDown() override
        {
            // FailIfHasErrorOutput();
        }
};

// INSTANTIATE_TEST_SUITE_P(AllApis, MaterialPassSystemTests, g_apiParams);

TEST_F(MaterialPassSystemTests, AddDynamicTarget_addsToEnabledPasses)
{
    
}

TEST_F(MaterialPassSystemTests, ClearDynamicTarget_clearsAllPasses)
{
    
}
