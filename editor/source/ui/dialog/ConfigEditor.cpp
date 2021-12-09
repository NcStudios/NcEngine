#include "ConfigEditor.h"
#include "ui/ImGuiUtility.h"
#include "utility/Output.h"
#include "debug/NcError.h"

#include <cstring>

namespace
{
    constexpr nc::Vector2 DialogSize{400.0f, 700.0f};
}

namespace nc::editor
{
    ConfigEditor::ConfigEditor()
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
        m_audioClipPathBuffer = m_config.projectSettings.audioClipsPath;
        m_concaveColliderPathBuffer = m_config.projectSettings.concaveCollidersPath;
        m_hullColliderPathBuffer = m_config.projectSettings.hullCollidersPath;
        m_meshPathBuffer = m_config.projectSettings.meshesPath;
        m_shaderPathBuffer = m_config.projectSettings.shadersPath;
        m_texturePathBuffer = m_config.projectSettings.texturesPath;
        m_cubeMapPathBuffer = m_config.projectSettings.cubeMapsPath;
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
            ImGui::PushItemWidth(75.0f);
            ImGui::Text("Project");
            ImGui::Indent();
            ImGui::PushItemWidth(250.0f);

            InputText("Project Name        ", &m_nameBuffer);
            InputText("Log Path            ", &m_nameBuffer);
            InputText("AudioClip Path      ", &m_audioClipPathBuffer);
            InputText("ConcaveCollider Path", &m_concaveColliderPathBuffer);
            InputText("HullCollider Path   ", &m_hullColliderPathBuffer);
            InputText("Mesh Path           ", &m_meshPathBuffer);
            InputText("Shader Path         ", &m_shaderPathBuffer);
            InputText("Texture Path        ", &m_texturePathBuffer);
            InputText("CubeMap Path        ", &m_cubeMapPathBuffer);

            ImGui::PopItemWidth();
            ImGui::Unindent();
            ImGui::Separator();

            ImGui::Text("Memory");
            ImGui::Indent();
            UnsignedInput(" Max Dynamic Colliders", &m_config.memorySettings.maxDynamicColliders);
            UnsignedInput(" Max Static Colliders", &m_config.memorySettings.maxStaticColliders);
            UnsignedInput(" Max NetworkDispatchers", &m_config.memorySettings.maxNetworkDispatchers);
            UnsignedInput(" Max ParticleEmitters", &m_config.memorySettings.maxParticleEmitters);
            UnsignedInput(" Max MeshRenderers", &m_config.memorySettings.maxRenderers);
            UnsignedInput(" Max Transforms", &m_config.memorySettings.maxTransforms);
            UnsignedInput(" Max PointLights", &m_config.memorySettings.maxPointLights);
            UnsignedInput(" Max Textures", &m_config.memorySettings.maxTextures);
            ImGui::Unindent();
            ImGui::Separator();

            ImGui::Text("Physics");
            ImGui::Indent();
            ImGui::DragFloat(" Fixed Step", &m_config.physicsSettings.fixedUpdateInterval, 0.00001f, 0.001f, 0.3333f, "%.5f");
            ImGui::DragFloat(" Worldspace Extent", &m_config.physicsSettings.worldspaceExtent, 1.0f, 10.0f, 5000.0f, "%.1f");
            ImGui::Unindent();
            ImGui::Separator();

            ImGui::Text("Graphics");
            ImGui::Indent();
            ImGui::Checkbox(" Use Native Resolution", &m_config.graphicsSettings.useNativeResolution);
            ImGui::Checkbox(" Use Native Resolution", &m_config.graphicsSettings.launchInFullscreen);
            UnsignedInput(" Screen Width", &m_config.graphicsSettings.screenWidth);
            UnsignedInput(" Screen Height", &m_config.graphicsSettings.screenHeight);
            UnsignedInput(" Target FPS", &m_config.graphicsSettings.targetFPS);

            ImGui::DragFloat(" Near Clip", &m_config.graphicsSettings.nearClip, 0.001f, 0.001f, 10.0f, "%.3f");
            ImGui::DragFloat(" Far Clip", &m_config.graphicsSettings.farClip, 1.0f, 1.0f, 5000.0f, "%.1f");
            ImGui::Checkbox(" Use Shadows", &m_config.graphicsSettings.useShadows);
            ImGui::Unindent();
            ImGui::Separator();
            ImGui::PopItemWidth();
            ImGui::Spacing();

            if(ImGui::Button("Save"))
            {
                m_config.projectSettings.projectName = m_nameBuffer;
                m_config.projectSettings.logFilePath = m_logPathBuffer;
                m_config.projectSettings.audioClipsPath = m_audioClipPathBuffer;
                m_config.projectSettings.concaveCollidersPath = m_concaveColliderPathBuffer;
                m_config.projectSettings.hullCollidersPath = m_hullColliderPathBuffer;
                m_config.projectSettings.meshesPath = m_meshPathBuffer;
                m_config.projectSettings.shadersPath = m_shaderPathBuffer;
                m_config.projectSettings.texturesPath = m_texturePathBuffer;
                m_config.projectSettings.cubeMapsPath = m_cubeMapPathBuffer;

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