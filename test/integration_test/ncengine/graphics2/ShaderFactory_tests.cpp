#include "DiligentEngineParameterizedFixture.inl"
#include "graphics2/diligent/ShaderFactory.h"

#include <filesystem>
#include <fstream>

constexpr auto g_goodSourceText = std::string_view{
R"(
struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(out PSOutput PSOut)
{
    PSOut.Color = float4(0, 0, 0, 0);
}
)"};

constexpr auto g_badSourceText = std::string_view{
R"(void main(in PSInput PSIn, out PSOutput PSOut)
{
}
)"};

constexpr auto g_shaderType = Diligent::SHADER_TYPE_PIXEL;
constexpr auto g_goodSource = std::span<const char>{g_goodSourceText.data(), g_goodSourceText.size()};
constexpr auto g_badSource = std::span<const char>{g_badSourceText};

class ShaderFactoryTest : public DiligentEngineParameterizedFixture
{
    protected:
        static inline std::filesystem::path testShaderPath = "";
        nc::graphics::ShaderFactory* uut = nullptr;

        static void SetUpTestSuite()
        {
            testShaderPath = std::filesystem::temp_directory_path() / "test_shader.psh";
            auto file = std::ofstream{testShaderPath};
            if (!file)
            {
                throw nc::NcError{"Failed to create temporary shader file"};
            }

            file << g_goodSourceText;
        }

        static void TearDownTestSuite()
        {
            if (!testShaderPath.empty())
            {
                std::filesystem::remove(testShaderPath);
            }
        }

        void SetUp() override
        {
            INITIALIZE_DILIGENT_FIXTURE;
            uut = &engine->GetShaderFactory();
        }

        void TearDown() override
        {
            FailIfHasErrorOutput();
        }
};

INSTANTIATE_TEST_SUITE_P(AllApis, ShaderFactoryTest, g_apiParams);

#ifdef NC_RUNTIME_SHADER_COMPILATION

TEST_P(ShaderFactoryTest, HasRuntimeCompilationSupport_hasRuntimeSupport_returnsTrue)
{
    EXPECT_TRUE(uut->HasRuntimeCompilationSupport());
}

TEST_P(ShaderFactoryTest, MakeShaderFromSource_goodSource_succeeds)
{
    EXPECT_NO_THROW(uut->MakeShaderFromSource(g_goodSource, "", g_shaderType));
}

TEST_P(ShaderFactoryTest, MakeShaderFromSource_invalidSyntax_throws)
{
    EXPECT_THROW(uut->MakeShaderFromSource(g_badSource, "", g_shaderType), nc::NcError);
    ClearErrorOutput();
}

TEST_P(ShaderFactoryTest, ReadShaderFile_validFile_producesValidSource)
{
    const auto source = nc::graphics::ReadShaderFile(testShaderPath.string());
    EXPECT_NO_THROW(uut->MakeShaderFromSource(source, "", g_shaderType));
}

TEST_P(ShaderFactoryTest, ReadShaderFile_badFile_throws)
{
    EXPECT_THROW(nc::graphics::ReadShaderFile("not_a_shader.psh"), nc::NcError);
}

#else

TEST_P(ShaderFactoryTest, HasRuntimeCompilationSupport_noRuntimeSupport_returnsFalse)
{
    EXPECT_FALSE(uut->HasRuntimeCompilationSupport());
}

TEST_P(ShaderFactoryTest, MakeShaderFromSource_noRuntimeSupport_throws)
{
    EXPECT_THROW(uut->MakeShaderFromSource(g_goodSource, "", g_shaderType), nc::NcError);
}

#endif
