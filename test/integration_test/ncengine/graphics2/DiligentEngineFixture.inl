#include "NcWindowStub.inl"
#include "graphics2/diligent/DiligentEngine.h"
#include "gtest/gtest.h"

#include <sstream>

// Base fixture for diligent tests parameterized on a list of graphics APIs.
class DiligentEngineFixture : public testing::Test
{
    protected:
        static inline auto s_diligentErrorOut = std::stringstream{};
        std::unique_ptr<nc::window::NcWindowStub> window;
        std::unique_ptr<nc::graphics::DiligentEngine> engine;

        DiligentEngineFixture()
        {
            ClearErrorOutput();
            window = std::make_unique<nc::window::NcWindowStub>(nc::window::WindowInfo{});
            auto engineCI = Diligent::EngineCreateInfo{};
            engineCI.Features.ShaderResourceRuntimeArrays = Diligent::DEVICE_FEATURE_STATE_ENABLED;
            engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
            engine = std::make_unique<nc::graphics::DiligentEngine>(
                engineCI,
                window->GetWindowHandle(),
                "dummyPath",
                &DiligentEngineFixture::LogCallback
            );
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

        // Create a simple graphics pipeline
        auto CreateTestGraphicsPipelineState(std::span<const char> vertexShaderSource,
                                             std::span<const char> pixelShaderSource,
                                             std::span<const Diligent::LayoutElement> inputLayout,
                                             std::span<Diligent::IPipelineResourceSignature*> signatures = {}) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
        {
            auto createInfo = Diligent::GraphicsPipelineStateCreateInfo{};
            createInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
            createInfo.PSODesc.Name = "Test PSO";
            createInfo.ppResourceSignatures = signatures.data();
            createInfo.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

            auto& shaderFactory = engine->GetShaderFactory();
            auto vertexShader = shaderFactory.MakeShaderFromSource(vertexShaderSource, "Test VS", Diligent::SHADER_TYPE_VERTEX, Diligent::SHADER_SOURCE_LANGUAGE_HLSL);
            auto pixelShader = shaderFactory.MakeShaderFromSource(pixelShaderSource, "Test PS", Diligent::SHADER_TYPE_PIXEL, Diligent::SHADER_SOURCE_LANGUAGE_HLSL);
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
