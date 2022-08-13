#include "Inspector.h"
#include "assets/AssetManifest.h"
#include "ui/Color.h"
#include "ui/ImGuiUtility.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

#include "DirectXMath.h"
#include "ecs/Registry.h"
#include "ecs/component/All.h"
#include "imgui/imgui.h"
#include "type/UIStream.h"

namespace
{
    using namespace nc::editor;

    template<class T>
    void ElementHeader(T* obj)
    {
        IMGUI_SCOPE_ID(StyleColor, 1, ImGuiCol_ButtonHovered, Color::Accent);
        IMGUI_SCOPE_ID(StyleColor, 2, ImGuiCol_ButtonActive, Color::AccentDark);
        SeparatorSpaced();
        ImGui::Button(nc::Type<T>::name, {-1,0});
        DragAndDropSource<T>(obj);
    }

    void xyzWidgetHeader(const char*)
    {
        IMGUI_SCOPE_ID(StyleColor, 1, ImGuiCol_Button, Color::Clear);
        IMGUI_SCOPE_ID(StyleColor, 2, ImGuiCol_ButtonHovered, Color::Clear);
        IMGUI_SCOPE_ID(StyleColor, 3, ImGuiCol_ButtonActive, Color::Clear);

        const ImVec2 buttonSize{DefaultItemWidth, 0};

        {
            IMGUI_SCOPE_ID(StyleColor, 4, ImGuiCol_Text, Color::Red);
            ImGui::Button("X##widgetHeader", buttonSize);
            SameLineSpaced();
        }

        {
            IMGUI_SCOPE_ID(StyleColor, 4, ImGuiCol_Text, Color::Green);
            ImGui::Button("Y##widgetHeader", buttonSize);
            SameLineSpaced();
        }

        {
            IMGUI_SCOPE_ID(StyleColor, 4, ImGuiCol_Text, Color::Blue);
            ImGui::Button("Z##widgetHeader", buttonSize);
        }
    }

    template<class Func>
    void AssetCombo(const char* label, const char* preview, const std::string& defaultPath, std::span<const Asset> assets, Func&& apply)
    {
        if(ImGui::BeginCombo(label, preview))
        {
            if(ImGui::Selectable("Default"))
                apply(defaultPath);

            for(const auto& asset : assets)
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                {
                    try
                    {
                        apply(asset.sourcePath.string());
                    }
                    catch(const std::runtime_error& e)
                    {
                        Output::LogError("Failure loading: " + asset.sourcePath.string(), "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }
    }
}

namespace nc::editor
{
    Inspector::Inspector(Registry* registry, AssetManifest* assetManifest)
        : m_registry{registry},
          m_assetManifest{assetManifest}
    {
    }

    void Inspector::InspectEntity(Entity entity)
    {
        ElementHeader(&entity);
        ImGui::Text("Tag     %s", m_registry->Get<Tag>(entity)->Value().data());
        ImGui::Text("Index   %d", entity.Index());
        ImGui::Text("Layer   %d", entity.Layer());
        ImGui::Text("Static  %d", entity.IsStatic());
        ImGui::Text("Persist %d", entity.IsPersistent());

        if(auto* transform  = m_registry->Get<Transform>(entity))         { DrawTransform(transform); }
        if(auto* camera     = m_registry->Get<Camera>(entity))            { DrawCamera(camera); }
        if(auto* renderer   = m_registry->Get<MeshRenderer>(entity))      { DrawMeshRenderer(renderer); }
        if(auto* light      = m_registry->Get<PointLight>(entity))        { DrawPointLight(light); }
        if(auto* body       = m_registry->Get<physics::PhysicsBody>(entity))       { DrawPhysicsBody(body); }
        if(auto* emitter    = m_registry->Get<ParticleEmitter>(entity))   { DrawParticleEmitter(emitter); }
        if(auto* dispatcher = m_registry->Get<NetworkDispatcher>(entity)) { DrawNetworkDispatcher(dispatcher); }
        if(auto* collider   = m_registry->Get<physics::Collider>(entity))          { DrawCollider(collider); }
        if(auto* collider   = m_registry->Get<ConcaveCollider>(entity))   { DrawConcaveCollider(collider); }
        if(auto* source     = m_registry->Get<AudioSource>(entity))       { DrawAudioSource(source); }
    }

    void Inspector::DrawAudioSource(AudioSource* audioSource)
    {
        ElementHeader(audioSource);

        ImGui::Text("Sound Clip");
        if(ImGui::BeginCombo("##audioclipcomboselect", audioSource->m_audioClipPath.c_str()))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::AudioClip))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                    audioSource->SetClip(asset.sourcePath.string());
            }

            ImGui::EndCombo();
        }
    }

    void Inspector::DrawCamera(Camera* camera)
    {
        ElementHeader(camera);
    }

    void Inspector::DrawCollider(physics::Collider* collider)
    {
        ElementHeader(collider);

        // collider model doesn't update/submit unless we tell it to
        collider->SetEditorSelection(true);

        const auto& info = collider->GetInfo();

        ImGui::SetNextItemWidth(75);
        if(ImGui::BeginCombo("Type##collidertypecombo", ToCString(info.type)))
        {
            if(ImGui::Selectable("Box") && info.type != physics::ColliderType::Box)
            {
                collider->SetProperties(physics::BoxProperties{.center = info.offset, .extents = info.scale});
            }
            if(ImGui::Selectable("Capsule") && info.type != physics::ColliderType::Capsule)
            {
                collider->SetProperties(physics::CapsuleProperties{.center = info.offset, .height = info.scale.y * 2.0f, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::Selectable("Sphere") && info.type != physics::ColliderType::Sphere)
            {
                collider->SetProperties(physics::SphereProperties{.center = info.offset, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::BeginMenu("Hull"))
            {
                for(const auto& asset : m_assetManifest->View(AssetType::HullCollider))
                {
                    if(ImGui::Selectable(asset.name.c_str()))
                    {
                        try
                        {
                            collider->SetProperties(physics::HullProperties{.assetPath = asset.ncaPath.value().string()});
                        }
                        catch(const std::runtime_error& e)
                        {
                            Output::LogError("Failure loading: " + asset.ncaPath.value().string(), "Exception: " + std::string{e.what()});
                        }
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Trigger##isTrigger", &collider->m_info.isTrigger);

        switch(collider->m_info.type)
        {
            case physics::ColliderType::Box:
            {
                DrawBoxColliderWidget(collider);
                break;
            }
            case physics::ColliderType::Capsule:
            {
                DrawCapsuleColliderWidget(collider);
                break;
            }
            case physics::ColliderType::Hull:
            {
                DrawHullColliderWidget(collider);
                break;
            }
            case physics::ColliderType::Sphere:
            {
                DrawSphereColliderWidget(collider);
            }
        }
    }

    void Inspector::DrawBoxColliderWidget(physics::Collider* collider)
    {
        xyzWidgetHeader("");
        InputVector3("Center", &collider->m_info.offset, 0.1f, -100.0f, 100.0f);
        InputVector3("Extents", &collider->m_info.scale, 0.1f, 0.01f, 1000.0f);
    }

    void Inspector::DrawCapsuleColliderWidget(physics::Collider* collider)
    {
        auto& info = collider->m_info;

        xyzWidgetHeader("");
        InputVector3("Center", &info.offset, 0.1f, -100.0f, 100.0f);
        float height = info.scale.y * 2.0f;
        float radius = info.scale.x * 0.5f;
        bool heightChanged = ImGui::DragFloat("Height", &height, 0.01f, radius, 100.0f);
        bool radiusChanged = ImGui::DragFloat("Radius", &radius, 0.01f, 0.01f, height);

        if(heightChanged || radiusChanged)
            collider->m_info.scale = Vector3{radius * 2.0f, height * 0.5f, radius * 2.0f};
    }

    void Inspector::DrawHullColliderWidget(physics::Collider* collider)
    {
        const char* assetPath = collider->m_info.hullAssetPath.c_str();
        auto assets = m_assetManifest->View(AssetType::HullCollider);
        AssetCombo("##hullcombo", assetPath, CubeHullColliderPath, assets, [collider](const std::string& path)
        {
            collider->SetProperties(physics::HullProperties{.assetPath = path});
        });
    }

    void Inspector::DrawSphereColliderWidget(physics::Collider* collider)
    {
        xyzWidgetHeader("      ");
        InputVector3("Center", &collider->m_info.offset, 0.1f, 100.0f, 100.0f);
        float radius = collider->m_info.scale.x * 0.5f;

        if(ImGui::DragFloat("Radius", &radius, 0.01f, 0.01f, 1000.0f))
            collider->m_info.scale = Vector3::Splat(radius) * 2.0f;
    }

    void Inspector::DrawConcaveCollider(ConcaveCollider* concaveCollider)
    {
        /** @todo Currently there is no way to change the asset for a conave collider */
        ElementHeader(concaveCollider);
        ImGui::Text("ConcaveCollider");
        ImGui::Text("  Path: %s", concaveCollider->GetPath().c_str());
    }

    void Inspector::DrawMeshRenderer(MeshRenderer* meshRenderer)
    {
        ElementHeader(meshRenderer);
        ImGui::Text("Mesh");
        const auto& material = meshRenderer->GetMaterial();
        if(ImGui::BeginCombo("##meshrenderermeshselectcombo", meshRenderer->GetMeshPath().c_str()))
        {
            if(ImGui::BeginMenu("Default"))
            {
                if(ImGui::Selectable("Cube"))    { meshRenderer->SetMesh(CubeMeshPath);    }
                if(ImGui::Selectable("Sphere"))  { meshRenderer->SetMesh(SphereMeshPath);  }
                if(ImGui::Selectable("Capsule")) { meshRenderer->SetMesh(CapsuleMeshPath); }
                if(ImGui::Selectable("Plane"))   { meshRenderer->SetMesh(PlaneMeshPath);   }
                ImGui::EndMenu();
            }

            for(const auto& asset : m_assetManifest->View(AssetType::Mesh))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                {
                    try
                    {
                        meshRenderer->SetMesh(asset.ncaPath.value().string());
                    }
                    catch(const std::runtime_error& e)
                    {
                        Output::LogError("Failure loading: " + asset.ncaPath.value().string(), "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Spacing(); ImGui::Spacing();

        auto textures = m_assetManifest->View(AssetType::Texture);

        ImGui::Text("Material");

        AssetCombo("Base##combo", material.baseColor.c_str(), DefaultBaseColorPath, textures, [meshRenderer](const std::string& path)
        {
            meshRenderer->SetBaseColor(path);
        });

        AssetCombo("Normal##combo", material.normal.c_str(), DefaultNormalPath, textures, [meshRenderer](const std::string& path)
        {
            meshRenderer->SetNormal(path);
        });

        AssetCombo("Roughness##combo", material.roughness.c_str(), DefaultRoughnessPath, textures, [meshRenderer](const std::string& path)
        {
            meshRenderer->SetRoughness(path);
        });

        AssetCombo("Metallic##combo", material.metallic.c_str(), DefaultRoughnessPath, textures, [meshRenderer](const std::string& path)
        {
            meshRenderer->SetMetallic(path);
        });
    }

    void Inspector::DrawNetworkDispatcher(NetworkDispatcher* networkDispatcher)
    {
        ElementHeader(networkDispatcher);
    }

    void Inspector::DrawParticleEmitter(ParticleEmitter* particleEmitter)
    {
        ElementHeader(particleEmitter);
    }

    void Inspector::DrawPhysicsBody(physics::PhysicsBody* physicsBody)
    {
        ElementHeader(physicsBody);
        IMGUI_SCOPE(ItemWidth, 50.0f);
        auto& properties = physicsBody->m_properties;

        ImGui::Checkbox("Use Gravity", &properties.useGravity);
        ImGui::Checkbox("Kinematic", &properties.isKinematic);

        if(physicsBody->ParentEntity().IsStatic())
        {
            ImGui::Text("0(Inf) Mass");
        }
        else
        {
            float mass = 1.0f / properties.mass;
            if(ImGui::DragFloat("Mass", &mass, 0.01f, 0.0f, 500.0f, "%.1f"))
            {
                // check if need to change inertia (maybe this doesn't matter since we're not doing physics)
                properties.mass = 1.0f / mass;
            }
        }

        ImGui::DragFloat("Drag", &properties.drag, 0.001f, 0.001f, 1.0f, "%.2f");
        ImGui::DragFloat("Angular Drag", &properties.angularDrag, 0.001f, 0.001f, 1.0f, "%.2f");
        ImGui::DragFloat("Restitution", &properties.restitution, 0.001f, 0.0f, 1.0f, "%.2f");
        ImGui::DragFloat("Friction", &properties.friction, 0.001f, 0.0f, 1.0f, "%.2f");
    }

    void Inspector::DrawPointLight(PointLight* pointLight)
    {
        ElementHeader(pointLight);
        auto info = pointLight->GetInfo();

        auto ambientResult = ImGui::ColorEdit3("Ambient Color", &info.ambient.x, ImGuiColorEditFlags_NoInputs);
        auto diffuseResult = ImGui::ColorEdit3("Diffuse Color", &info.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
        auto intensityResult = ImGui::DragFloat("Intensity", &info.diffuseIntensity, 1.0f, 0.0f, 600.0f, "%.2f");

        if (ambientResult || diffuseResult || intensityResult)
        {
            pointLight->SetInfo(info);
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

        ElementHeader(transform);
        xyzWidgetHeader("");

        auto posResult = InputPosition("Pos", &pos);
        auto rotResult = InputAngles("Rot", &angles);
        auto sclResult = InputScale("Scl", &scl);

        if(posResult)
            transform->SetPosition(pos);
        if(rotResult)
            transform->SetRotation(angles);
        if(sclResult)
            transform->SetScale(scl);
    }
}