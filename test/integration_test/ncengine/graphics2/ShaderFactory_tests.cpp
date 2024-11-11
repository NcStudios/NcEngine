#include "DiligentEngineFixture.inl"
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

class ShaderFactoryTest : public DiligentEngineFixture
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

        ShaderFactoryTest()
        {
            uut = &engine->GetShaderFactory();
        }

        ~ShaderFactoryTest()
        {
            FailIfHasErrorOutput();
        }
};

#ifdef NC_RUNTIME_SHADER_COMPILATION

TEST_F(ShaderFactoryTest, RuntimeSupport_happyPaths_succeed)
{
    EXPECT_TRUE(uut->HasRuntimeCompilationSupport());

    EXPECT_NO_THROW(uut->MakeShaderFromSource(g_goodSource, "", g_shaderType));

    const auto source = nc::graphics::ReadShaderFile(testShaderPath.string());
    EXPECT_NO_THROW(uut->MakeShaderFromSource(source, "", g_shaderType));
}

TEST_F(ShaderFactoryTest, RuntimeSupport_failurePaths_throw)
{
    EXPECT_THROW(uut->MakeShaderFromSource(g_badSource, "", g_shaderType), nc::NcError);
    EXPECT_THROW(nc::graphics::ReadShaderFile("not_a_shader.psh"), nc::NcError);
    ClearErrorOutput();
}

#else

TEST_F(ShaderFactoryTest, NoRuntimeSupport_failurePaths_throw)
{
    EXPECT_FALSE(uut->HasRuntimeCompilationSupport());
    EXPECT_THROW(uut->MakeShaderFromSource(g_goodSource, "", g_shaderType), nc::NcError);
}

#endif
