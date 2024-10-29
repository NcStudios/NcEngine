#include "NcWindowStub.inl"
#include "graphics2/diligent/DiligentEngine.h"
#include "gtest/gtest.h"

#include "ncengine/config/Config.h"
#include "ncengine/graphics/NcGraphics.h"

#include <sstream>

// APIs supported on local machine
const auto g_supportedApis = nc::graphics::GetSupportedApis();

// All APIs for test instantiation (for use with INSTANTIATE_TEST_SUITE_P)
const auto g_apiParams = testing::Values("vulkan", "d3d12");

// Helper macro to initialize or skip a test case based on support for the current test API
#define INITIALIZE_DILIGENT_FIXTURE                                        \
if (!IsCurrentTestApiSupported())                                          \
    GTEST_SKIP() << "Skipping test for unsupported API: " << GetTestApi(); \
DiligentEngineParameterizedFixture::SetUp();                               \

// Base fixture for diligent tests parameterized on a list of graphics APIs.
class DiligentEngineParameterizedFixture : public testing::TestWithParam<std::string_view>
{
    protected:
        static inline auto s_diligentErrorOut = std::stringstream{};
        bool isHeadless;
        std::unique_ptr<nc::window::NcWindowStub> window;
        std::unique_ptr<nc::graphics::DiligentEngine> engine;

        DiligentEngineParameterizedFixture(bool headless = true)
            : isHeadless{headless}
        {
            ClearErrorOutput();
        }

        // Get the API used for the current test
        auto GetTestApi() -> std::string_view
        {
            return GetParam();
        }

        // Check if the API used for the current test is supported on the local machine
        auto IsCurrentTestApiSupported() -> bool
        {
            return std::ranges::contains(g_supportedApis, GetTestApi());
        }

        // Fail the current test case if diligent logged any warnings/errors
        void FailIfHasErrorOutput()
        {
            const auto errors = GetErrorOutput();
            if (!errors.empty())
            {
                FAIL() << "Failure detected in Diligent error output:\n" << errors;
            }
        }

        // Get diligent warning/error output for the current test case
        auto GetErrorOutput() -> std::string
        {
            return s_diligentErrorOut.str();
        }

        // Clear errors for the current test case
        void ClearErrorOutput()
        {
            s_diligentErrorOut.str("");
        }

        // Initialize the fixture
        // note: It is the caller's responsibility to ensure the current API is supported prior to calling this. Use
        //       INITIALIZE_DILIGENT_FIXTURE to conditionally initialize/skip the current test based on API support.
        void SetUp() override
        {
            window = std::make_unique<nc::window::NcWindowStub>(
                nc::window::WindowInfo{
                    .isHeadless = isHeadless
                }
            );

            auto engineCI = Diligent::EngineCreateInfo{};
            engineCI.Features.ShaderResourceRuntimeArrays = Diligent::DEVICE_FEATURE_STATE_ENABLED;
            engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;

            auto graphicsSettings = nc::config::GraphicsSettings();
            graphicsSettings.isHeadless = isHeadless;
            graphicsSettings.api = GetTestApi();

            engine = std::make_unique<nc::graphics::DiligentEngine>(
                graphicsSettings,
                engineCI,
                window->GetWindowHandle(),
                g_supportedApis,
                &DiligentEngineParameterizedFixture::LogCallback
            );
        }

        // Create a simple graphics pipeline
        auto CreateTestGraphicsPipelineState(std::span<const char> vertexShaderSource,
                                             std::span<const char> pixelShaderSource,
                                             std::span<const Diligent::LayoutElement> inputLayout) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
        {
            auto createInfo = Diligent::GraphicsPipelineStateCreateInfo{};
            createInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
            createInfo.PSODesc.Name = "Test PSO";

            auto& shaderFactory = engine->GetShaderFactory();
            auto vertexShader = shaderFactory.MakeShaderFromSource(vertexShaderSource, "Test VS", Diligent::SHADER_TYPE_VERTEX);
            auto pixelShader = shaderFactory.MakeShaderFromSource(pixelShaderSource, "Test PS", Diligent::SHADER_TYPE_PIXEL);
            createInfo.pVS = vertexShader;
            createInfo.pPS = pixelShader;

            const auto& scDesc = engine->GetSwapChain().GetDesc();
            createInfo.GraphicsPipeline.RTVFormats[0] = scDesc.ColorBufferFormat;
            createInfo.GraphicsPipeline.DSVFormat = scDesc.DepthBufferFormat;
            createInfo.GraphicsPipeline.NumRenderTargets = 1;
            createInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            createInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;
            createInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
            createInfo.GraphicsPipeline.InputLayout.LayoutElements = inputLayout.data();
            createInfo.GraphicsPipeline.InputLayout.NumElements = static_cast<uint32_t>(inputLayout.size());

            auto pso = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
            engine->GetDevice().CreateGraphicsPipelineState(createInfo, &pso);
            if (!pso)
            {
                throw nc::NcError{"Failed to create test pipeline state object"};
            }

            return pso;
        }

        private:
            static void LogCallback(Diligent::DEBUG_MESSAGE_SEVERITY severity,
                                    const char* msg,
                                    const char* func,
                                    const char* file,
                                    int line)
            {
                if (severity == Diligent::DEBUG_MESSAGE_SEVERITY_INFO)
                {
                    return;
                }

                s_diligentErrorOut << "File: "     << (file ? file : "null") << '\n'
                                << "Function: " << (func ? func : "null") << '\n'
                                << "Line: "     << line                   << '\n'
                                << '\t'         << msg                    << '\n';
            }
};
