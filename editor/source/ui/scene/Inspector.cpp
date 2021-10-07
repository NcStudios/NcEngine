#include "Inspector.h"
#include "assets/AssetManifest.h"
#include "directx/math/DirectXMath.h"
#include "imgui/imgui.h"

namespace
{
    constexpr float defaultItemWidth = 50.0f;

    void ElementHeader(const char* name)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text(name);
    }

    namespace color
    {
        const auto White = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
        const auto Black = ImVec4{0.0f, 0.0f, 0.0f, 1.0f};
        const auto Clear = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};
        const auto Red =   ImVec4{1.0f, 0.2f, 0.1f, 1.0f};
        const auto Green = ImVec4{0.0f, 1.0f, 0.0f, 1.0f};
        const auto Blue =  ImVec4{0.0f, 0.4f, 1.0f, 1.0f};
    }

    bool floatWidget(const char* label, const char* id, float* item, float dragSpeed = 0.1f, float min = 0.1f, float max = 10.0f, const char* fmt = "%.1f")
    {
        ImGui::PushID(id);
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(defaultItemWidth);
        auto result = ImGui::DragFloat("", item, dragSpeed, min, max, fmt);
        ImGui::PopID();
        return result;
    };

    void textBlockWidget(const char* label, ImVec2 size, ImVec4 bgColor, ImVec4 textColor)
    {
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, textColor);
        ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
    }

    void xyzWidgetHeader(const char* frontPadding)
    {
        const ImVec2 buttonSize{defaultItemWidth, 0};
        ImGui::Text(frontPadding); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
        textBlockWidget("X##widgetHeader", buttonSize, color::Clear, color::Red); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
        textBlockWidget("Y##widgetHeader", buttonSize, color::Clear, color::Green); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
        textBlockWidget("Z##widgetHeader", buttonSize, color::Clear, color::Blue);
    }

    bool xyzWidget(const char* groupLabel, const char* id, float* x, float* y, float* z, float min = -50.0f, float max = 50.0f)
    {
        ImGui::PushID(id);
        ImGui::Text(groupLabel); ImGui::SameLine();
        auto xResult = floatWidget("", "x", x, 0.1f, min, max, "%.1f"); ImGui::SameLine();
        auto yResult = floatWidget("", "y", y, 0.1f, min, max, "%.1f"); ImGui::SameLine();
        auto zResult = floatWidget("", "z", z, 0.1f, min, max, "%.1f");
        ImGui::PopID();

        return xResult || yResult || zResult;
    };

    void columnHeaderWidget(const char* frontPadding, const char* label1, const char* label2, const char* label3 = nullptr)
    {
        const ImVec2 buttonSize{defaultItemWidth, 0};
        ImGui::PushItemWidth(defaultItemWidth);
        ImGui::Text(frontPadding); ImGui::SameLine();
        textBlockWidget(label1, buttonSize, color::Clear, color::White); ImGui::SameLine();
        textBlockWidget(label2, buttonSize, color::Clear, color::White); ImGui::SameLine();
        if(label3)
            textBlockWidget(label3, buttonSize, color::Clear, color::White);
        ImGui::PopItemWidth();
    }
}

namespace nc::editor
{
    Inspector::Inspector(registry_type* registry, AssetManifest* assetManifest)
        : m_registry{registry},
          m_assetManifest{assetManifest}
    {
    }

    void Inspector::InspectEntity(Entity entity)
    {
        ImGui::Separator();
        ImGui::Text("Tag     %s", m_registry->Get<Tag>(entity)->Value().data());
        ImGui::Text("Index   %d", entity.Index());
        ImGui::Text("Layer   %d", entity.Layer());
        ImGui::Text("Static  %s", entity.IsStatic() ? "True" : "False");

        if (auto* transform = m_registry->Get<Transform>(entity); transform)
            DrawTransform(transform);

        if (auto* meshRenderer = m_registry->Get<MeshRenderer>(entity))
            DrawMeshRenderer(meshRenderer);

        if (auto* pointLight = m_registry->Get<PointLight>(entity))
            DrawPointLight(pointLight);

        if (auto* body = m_registry->Get<PhysicsBody>(entity))
            DrawPhysicsBody(body);

        if (auto* emitter = m_registry->Get<ParticleEmitter>(entity))
            DrawParticleEmitter(emitter);

        if (auto* dispatcher = m_registry->Get<NetworkDispatcher>(entity))
            DrawNetworkDispatcher(dispatcher);

        if (auto* col = m_registry->Get<Collider>(entity); col)
            DrawCollider(col);

        if(auto* col = m_registry->Get<ConcaveCollider>(entity); col)
            DrawConcaveCollider(col);

        if(auto* src = m_registry->Get<AudioSource>(entity); src)
            DrawAudioSource(src);
        
        // for(const auto& comp : m_registry->Get<AutoComponentGroup>(entity)->GetAutoComponents())
        //     AutoComponentElement(comp);
    
        ImGui::Separator();
    }

    void Inspector::DrawAudioSource(AudioSource*)
    {
        ElementHeader("AudioSource");
    }

    void Inspector::DrawCollider(Collider* collider)
    {
        // collider model doesn't update/submit unless we tell it to
        collider->SetEditorSelection(true);

        const auto& info = collider->GetInfo();
        auto& offset = collider->m_info.offset;

        ElementHeader("Collider");
        ImGui::BeginGroup();
        ImGui::Indent();

        ImGui::Text("Type   ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(75);

        if(ImGui::BeginCombo("##collidertypecombo", ToCString(info.type)))
        {
            if(ImGui::Selectable("Box") && info.type != ColliderType::Box)
            {
                collider->SetProperties(BoxProperties{.center = info.offset, .extents = info.scale});
            }
            if(ImGui::Selectable("Capsule"))
            {
                collider->SetProperties(CapsuleProperties{.center = info.offset, .height = info.scale.y * 2.0f, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::Selectable("Sphere"))
            {
                collider->SetProperties(SphereProperties{.center = info.offset, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::BeginMenu("Hull"))
            {
                for(const auto& asset : m_assetManifest->View(AssetType::HullCollider))
                {
                    if(ImGui::Selectable(asset.name.c_str()))
                    {
                        collider->SetProperties(HullProperties{.assetPath = asset.ncaPath.value().string()});
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Trigger");
        ImGui::SameLine();
        ImGui::Checkbox("##isTrigger", &collider->m_info.isTrigger);

        switch(collider->m_info.type)
        {
            case ColliderType::Box:
            {
                xyzWidgetHeader("       ");
                xyzWidget("Center ", "colliderpos", &offset.x, &offset.y, &offset.z, -100.0f, 100.0f);
                auto& extents = collider->m_info.scale;
                xyzWidget("Extents", "colliderscl", &extents.x, &extents.y, &extents.z, 0.01, 1000.0f);
                break;
            }
            case ColliderType::Capsule:
            {
                xyzWidgetHeader("");
                xyzWidget("Center", "colliderpos", &offset.x, &offset.y, &offset.z, -100.0f, 100.0f);
                float height = collider->m_info.scale.y * 2.0f;
                float radius = collider->m_info.scale.x * 0.5f;
                bool heightChanged = floatWidget("Height", "colliderheight", &height, 0.01f, radius, 100.0f);
                bool radiusChanged = floatWidget("Radius", "colliderradius", &radius, 0.01f, 0.01f, height);
                if(heightChanged || radiusChanged)
                    collider->m_info.scale = Vector3{radius * 2.0f, height * 0.5f, radius * 2.0f};
                break;
            }
            case ColliderType::Hull:
            {
                /** @todo */

                if(ImGui::BeginCombo("##colliderhullselectcombo", info.hullAssetPath.c_str()))
                {
                    for(const auto& asset : m_assetManifest->View(AssetType::HullCollider))
                    {
                        if(ImGui::Selectable(asset.name.c_str()))
                        {
                            collider->SetProperties(HullProperties{.assetPath = asset.ncaPath.value().string()});
                        }
                    }

                    ImGui::EndCombo();
                }
            
                break;
            }
            case ColliderType::Sphere:
            {
                xyzWidgetHeader("      ");
                xyzWidget("Center", "collidercenter", &offset.x, &offset.y, &offset.z, 100.0f, 100.0f);
                float radius = collider->m_info.scale.x * 0.5f;
                if(floatWidget("Radius", "colliderradius", &radius, 0.01f, 0.01f, 1000.0f))
                    collider->m_info.scale = Vector3::Splat(radius) * 2.0f;
                break;
            }
        }

        ImGui::Unindent();
        ImGui::EndGroup();
    }

    void Inspector::DrawConcaveCollider(ConcaveCollider* concaveCollider)
    {
        ElementHeader("ConcaveCollider");
        ImGui::BeginGroup();
        ImGui::Indent();
        ImGui::Text("ConcaveCollider");
        ImGui::Text("  Path: %s", concaveCollider->GetPath().c_str());
        ImGui::Unindent();
        ImGui::EndGroup();
    }

    void Inspector::DrawMeshRenderer(MeshRenderer* meshRenderer)
    {
        const auto& material = meshRenderer->GetMaterial();

        ElementHeader("MeshRenderer");
        ImGui::BeginGroup();
        ImGui::Indent();
        ImGui::Text("Mesh: ???");
        ImGui::Text("Material");

        ImGui::Text("Base:");
        if(ImGui::BeginCombo("##meshrendererbaseselectcombo", material.baseColor.c_str()))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                    meshRenderer->SetBaseColor(asset.sourcePath.string());
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Normal");
        if(ImGui::BeginCombo("##meshrenderernormalselectcombo", material.normal.c_str()))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                    meshRenderer->SetNormal(asset.sourcePath.string());
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Roughness");
        if(ImGui::BeginCombo("##meshrendererroughnessselectcombo", material.roughness.c_str()))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                    meshRenderer->SetRoughness(asset.sourcePath.string());
            }

            ImGui::EndCombo();
        }

        ImGui::Unindent();
        ImGui::EndGroup();
    }

    void Inspector::DrawNetworkDispatcher(NetworkDispatcher*)
    {
        ElementHeader("NetworkDispatcher");
    }

    void Inspector::DrawParticleEmitter(ParticleEmitter*)
    {
        ElementHeader("ParticleEmitter");
    }

    void Inspector::DrawPhysicsBody(PhysicsBody* physicsBody)
    {
        auto& properties = physicsBody->m_properties;

        ElementHeader("PhysicsBody");
        ImGui::BeginGroup();
            ImGui::Indent();

            ImGui::Text("Use Gravity");
            ImGui::SameLine();
            ImGui::Checkbox("##gravitybox", &properties.useGravity);

            ImGui::Text("Kinematic  ");
            ImGui::SameLine();
            ImGui::Checkbox("##kinematicbox", &properties.isKinematic);

            if(physicsBody->GetParentEntity().IsStatic())
            {
                ImGui::Text("Mass               0(Inf)");
            }
            else
            {
                float mass = 1.0f / properties.mass;
                if(floatWidget("Mass             ", "pbmass", &mass, 0.01f, 0.0f, 500.0f, "%.1f"))
                {
                    // check if need to change inertia (maybe this doesn't matter since we're not doing physics)
                    properties.mass = 1.0f / mass;
                }
            }

            floatWidget("Drag             ", "pbdrag", &properties.drag, 0.001f, 0.001f, 1.0f, "%.2f");
            floatWidget("Angular Drag     ", "pbangdrag", &properties.angularDrag, 0.001f, 0.001f, 1.0f, "%.2f");
            floatWidget("Restitution      ", "pbrest", &properties.restitution, 0.001f, 0.0f, 1.0f, "%.2f");
            floatWidget("Friction         ", "pbfriction", &properties.friction, 0.001f, 0.0f, 1.0f, "%.2f");
        ImGui::EndGroup();
    }

    void Inspector::DrawPointLight(PointLight* pointLight)
    {
        const float dragSpeed = 1.0f;
        auto& info = pointLight->GetInfo();

        Vector3 ambient = info.ambient;
        Vector3 diffuse = info.diffuseColor;
        float diffuseIntensity = info.diffuseIntensity;
        float attConst = info.attConst;
        float attLin = info.attLin;
        float attQuad = info.attQuad;

        ElementHeader("PointLight");
        ImGui::BeginGroup();
        ImGui::Indent();
            ImGui::Text("Ambient    ");
            ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  auto ambientResult = ImGui::ColorEdit3("##ambcolor", &ambient.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Diffuse    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine(); auto diffuseResult = ImGui::ColorEdit3("##difcolor", &diffuse.x, ImGuiColorEditFlags_NoInputs);
                auto diffuseIntensityResult = floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 600.0f, "%.2f");
            ImGui::Unindent();
            ImGui::Text("Attenuation");
                columnHeaderWidget("", "Const", "Lin", "Quad");
                xyzWidget("", "att", &attConst, &attLin, &attQuad, 0.01f, 1.0f);
                
            auto buttonSize = ImVec2{ImGui::GetWindowWidth() - 20, 18};
            if(ImGui::Button("Serialize", buttonSize))
                SerializeToFile("nc/PointLightData.txt", info);
        ImGui::Unindent();
        ImGui::EndGroup();

        auto pointLightInfo = info;

        if (ambientResult) pointLightInfo.ambient = ambient;
        if (diffuseResult) pointLightInfo.diffuseColor = diffuse;
        if (diffuseIntensityResult) pointLightInfo.diffuseIntensity = diffuseIntensity;

        if (ambientResult || diffuseResult || diffuseIntensityResult)
        {
            pointLight->SetInfo(pointLightInfo);
        }
    }

    void Inspector::DrawTransform(Transform* transform)
    {
        auto& worldMatrix = transform->m_localMatrix;
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, worldMatrix);
        Vector3 scl, pos;
        auto rot = Quaternion::Identity();
        DirectX::XMStoreVector3(&scl, scl_v);
        DirectX::XMStoreQuaternion(&rot, rot_v);
        DirectX::XMStoreVector3(&pos, pos_v);
        auto angles = rot.ToEulerAngles();

        ElementHeader("Transform");
        ImGui::BeginGroup();
        ImGui::Indent();
        xyzWidgetHeader("   ");
        auto posResult = xyzWidget("Pos", "transformpos", &pos.x, &pos.y, &pos.z);
        auto rotResult = xyzWidget("Rot", "transformrot", &angles.x, &angles.y, &angles.z, std::numbers::pi * -2.0f, std::numbers::pi * 2.0f);
        auto sclResult = xyzWidget("Scl", "transformscl", &scl.x, &scl.y, &scl.z);
        ImGui::Unindent();
        ImGui::EndGroup();

        if(posResult)
            transform->SetPosition(pos);
        if(rotResult)
            transform->SetRotation(angles);
        if(sclResult)
            transform->SetScale(scl);
    }
}