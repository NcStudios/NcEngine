#include "ConfigEditor.h"
#include "ui/ImGuiUtility.h"
#include "utility/Output.h"
#include "debug/NcError.h"

#include <cstring>

namespace
{
    constexpr nc::Vector2 DialogSize{500.0f, 1000.0f};
}

namespace nc::editor
{
    ConfigEditor::ConfigEditor(const std::filesystem::path& configPath)
        : DialogFixedCentered("Config Editor", DialogSize),
          m_config{},
          m_addDialog{},
          m_path{},
          m_nameBuffer{},
          m_logPathBuffer{},
          m_audioClipPathBuffer{},
          m_concaveColliderPathBuffer{},
          m_hullColliderPathBuffer{},
          m_meshPathBuffer{},
          m_shaderPathBuffer{},
          m_texturePathBuffer{},
          m_cubeMapPathBuffer{}
    {
        try
        {
            m_config = config::Load(configPath.string());
            m_path = configPath;
        }
        catch(const NcError& e)
        {
            Output::LogError("Failure opening config", std::string{"Exception: "} + e.what());
            return;
        }
    }

    void ConfigEditor::Open(const std::filesystem::path& configPath)
    {
        try
        {
            m_config = config::Load(configPath.string());
        }
        catch(const NcError& e)
        {
            Output::LogError("Failure opening config", std::string{"Exception: "} + e.what());
            return;
        }

        m_isOpen = true;
        m_path = configPath;
        m_addDialog(this);

        m_nameBuffer = m_config.projectSettings.projectName;
        m_logPathBuffer = m_config.projectSettings.logFilePath;
        m_audioClipPathBuffer = m_config.assetSettings.audioClipsPath;
        m_concaveColliderPathBuffer = m_config.assetSettings.concaveCollidersPath;
        m_hullColliderPathBuffer = m_config.assetSettings.hullCollidersPath;
        m_meshPathBuffer = m_config.assetSettings.meshesPath;
        m_shaderPathBuffer = m_config.assetSettings.shadersPath;
        m_texturePathBuffer = m_config.assetSettings.texturesPath;
        m_cubeMapPathBuffer = m_config.assetSettings.cubeMapsPath;
    }

    void ConfigEditor::Draw()
    {
        if(!m_isOpen) return;

        auto UnsignedInput = [](const char* label, unsigned* value)
        {
            int v = *value;
            ImGui::DragInt(label, &v, 1, 0, 500000);
            *value = static_cast<unsigned>(v);
        };

        if(BeginWindow())
        {
            IMGUI_SCOPE_ID(ItemWidth, 1, DefaultItemWidth);
            ImGui::Text("Project");
            {
                IMGUI_SCOPE_ID(ItemWidth, 2, 200.0f);
                IMGUI_SCOPE(Indent);
                InputText("Project Name", &m_nameBuffer);
                InputText("Log Path", &m_nameBuffer);
            }

            ImGui::Separator();
            ImGui::Text("Asset");
            {
                IMGUI_SCOPE_ID(ItemWidth, 2, 275.0f);
                IMGUI_SCOPE(Indent);
                InputText("AudioClip Path", &m_audioClipPathBuffer);
                InputText("ConcaveCollider Path", &m_concaveColliderPathBuffer);
                InputText("HullCollider Path", &m_hullColliderPathBuffer);
                InputText("Mesh Path", &m_meshPathBuffer);
                InputText("Shader Path", &m_shaderPathBuffer);
                InputText("Texture Path", &m_texturePathBuffer);
                InputText("CubeMap Path", &m_cubeMapPathBuffer);
            }

            ImGui::Separator();
            ImGui::Text("Memory");
            {
                IMGUI_SCOPE(Indent);
                UnsignedInput("Max Dynamic Colliders", &m_config.memorySettings.maxDynamicColliders);
                UnsignedInput("Max Static Colliders", &m_config.memorySettings.maxStaticColliders);
                UnsignedInput("Max NetworkDispatchers", &m_config.memorySettings.maxNetworkDispatchers);
                UnsignedInput("Max ParticleEmitters", &m_config.memorySettings.maxParticleEmitters);
                UnsignedInput("Max MeshRenderers", &m_config.memorySettings.maxRenderers);
                UnsignedInput("Max Transforms", &m_config.memorySettings.maxTransforms);
                UnsignedInput("Max PointLights", &m_config.memorySettings.maxPointLights);
                UnsignedInput("Max Textures", &m_config.memorySettings.maxTextures);
            }

            ImGui::Separator();
            ImGui::Text("Physics");
            {
                IMGUI_SCOPE(Indent);
                ImGui::DragFloat("Fixed Step", &m_config.physicsSettings.fixedUpdateInterval, 0.00001f, 0.001f, 0.3333f, "%.5f");
                ImGui::DragFloat("Worldspace Extent", &m_config.physicsSettings.worldspaceExtent, 1.0f, 10.0f, 5000.0f, "%.1f");
            }
            
            ImGui::Separator();
            ImGui::Text("Graphics");
            {
                IMGUI_SCOPE(Indent);
                ImGui::Checkbox("Use Native Resolution", &m_config.graphicsSettings.useNativeResolution);
                ImGui::Checkbox("Use Native Resolution", &m_config.graphicsSettings.launchInFullscreen);
                UnsignedInput("Screen Width", &m_config.graphicsSettings.screenWidth);
                UnsignedInput("Screen Height", &m_config.graphicsSettings.screenHeight);
                UnsignedInput("Target FPS", &m_config.graphicsSettings.targetFPS);

                ImGui::DragFloat("Near Clip", &m_config.graphicsSettings.nearClip, 0.001f, 0.001f, 10.0f, "%.3f");
                ImGui::DragFloat("Far Clip", &m_config.graphicsSettings.farClip, 1.0f, 1.0f, 5000.0f, "%.1f");
                ImGui::Checkbox("Use Shadows", &m_config.graphicsSettings.useShadows);
            }

            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::Button("Save"))
            {
                m_config.projectSettings.projectName = m_nameBuffer;
                m_config.projectSettings.logFilePath = m_logPathBuffer;
                m_config.assetSettings.audioClipsPath = m_audioClipPathBuffer;
                m_config.assetSettings.concaveCollidersPath = m_concaveColliderPathBuffer;
                m_config.assetSettings.hullCollidersPath = m_hullColliderPathBuffer;
                m_config.assetSettings.meshesPath = m_meshPathBuffer;
                m_config.assetSettings.shadersPath = m_shaderPathBuffer;
                m_config.assetSettings.texturesPath = m_texturePathBuffer;
                m_config.assetSettings.cubeMapsPath = m_cubeMapPathBuffer;

                if(config::Validate(m_config))
                {
                    m_isOpen = false;

                    try
                    {
                        config::Save(m_path.string(), m_config);
                    }
                    catch(const NcError& e)
                    {
                        Output::LogError("Failure saving config", e.what());
                    }
                }
                else
                {
                    Output::LogError("Invalid config");
                }
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                m_isOpen = false;
            }
        }

        EndWindow();
    }

    void ConfigEditor::RegisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog)
    {
        m_addDialog = std::move(registerDialog);
    }
}