#include "Inspector.h"
#include "assets/AssetManifest.h"
#include "ui/Color.h"
#include "ui/ImGuiUtility.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"
#include "directx/math/DirectXMath.h"
#include "imgui/imgui.h"

#include "ecs/Registry.h"
#include "ecs/component/AudioSource.h"
#include "ecs/component/Camera.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/NetworkDispatcher.h"
#include "ecs/component/ParticleEmitter.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/PointLight.h"
#include "ecs/component/Transform.h"

namespace
{
    using namespace nc::editor;

    template<class T>
    void ElementHeader(T* obj)
    {
        IMGUI_SCOPE_ID(StyleColor, 1, ImGuiCol_ButtonHovered, Color::Accent);
        IMGUI_SCOPE_ID(StyleColor, 2, ImGuiCol_ButtonActive, Color::AccentDark);
        SeparatorSpaced();
        ImGui::Button(nc::TypeInfo<T>::name, {-1,0});
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
        if(auto* body       = m_registry->Get<PhysicsBody>(entity))       { DrawPhysicsBody(body); }
        if(auto* emitter    = m_registry->Get<ParticleEmitter>(entity))   { DrawParticleEmitter(emitter); }
        if(auto* dispatcher = m_registry->Get<NetworkDispatcher>(entity)) { DrawNetworkDispatcher(dispatcher); }
        if(auto* collider   = m_registry->Get<Collider>(entity))          { DrawCollider(collider); }
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

    void Inspector::DrawCollider(Collider* collider)
    {
        ElementHeader(collider);

        // collider model doesn't update/submit unless we tell it to
        collider->SetEditorSelection(true);

        const auto& info = collider->GetInfo();

        ImGui::SetNextItemWidth(75);
        if(ImGui::BeginCombo("Type##collidertypecombo", ToCString(info.type)))
        {
            if(ImGui::Selectable("Box") && info.type != ColliderType::Box)
            {
                collider->SetProperties(BoxProperties{.center = info.offset, .extents = info.scale});
            }
            if(ImGui::Selectable("Capsule") && info.type != ColliderType::Capsule)
            {
                collider->SetProperties(CapsuleProperties{.center = info.offset, .height = info.scale.y * 2.0f, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::Selectable("Sphere") && info.type != ColliderType::Sphere)
            {
                collider->SetProperties(SphereProperties{.center = info.offset, .radius = info.scale.x * 0.5f});
            }
            if(ImGui::BeginMenu("Hull"))
            {
                for(const auto& asset : m_assetManifest->View(AssetType::HullCollider))
                {
                    if(ImGui::Selectable(asset.name.c_str()))
                    {
                        try
                        {
                            collider->SetProperties(HullProperties{.assetPath = asset.ncaPath.value().string()});
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
            case ColliderType::Box:
            {
                DrawBoxColliderWidget(collider);
                break;
            }
            case ColliderType::Capsule:
            {
                DrawCapsuleColliderWidget(collider);
                break;
            }
            case ColliderType::Hull:
            {
                DrawHullColliderWidget(collider);
                break;
            }
            case ColliderType::Sphere:
            {
                DrawSphereColliderWidget(collider);
            }
        }
    }

    void Inspector::DrawBoxColliderWidget(Collider* collider)
    {
        xyzWidgetHeader("");
        InputVector3("Center", &collider->m_info.offset, 0.1f, -100.0f, 100.0f);
        InputVector3("Extents", &collider->m_info.scale, 0.1f, 0.01f, 1000.0f);
    }

    void Inspector::DrawCapsuleColliderWidget(Collider* collider)
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

    void Inspector::DrawHullColliderWidget(Collider* collider)
    {
        /** @todo */

        if(ImGui::BeginCombo("##colliderhullselectcombo", collider->m_info.hullAssetPath.c_str()))
        {
            for(const auto& asset : m_assetManifest->View(AssetType::HullCollider))
            {
                if(ImGui::Selectable(asset.name.c_str()))
                {
                    try
                    {
                        collider->SetProperties(HullProperties{.assetPath = asset.ncaPath.value().string()});
                    }
                    catch(const std::runtime_error& e)
                    {
                        Output::LogError("Failure loading: " + asset.ncaPath.value().string(), "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }
    }

    void Inspector::DrawSphereColliderWidget(Collider* collider)
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

        ImGui::Text("Material");
        if(ImGui::BeginCombo("Base##meshrendererbaseselectcombo", material.baseColor.c_str()))
        {
            if(ImGui::Selectable("Default"))
                meshRenderer->SetBaseColor(DefaultBaseColorPath);
            
            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                {
                    try
                    {
                        meshRenderer->SetBaseColor(asset.sourcePath.string());
                    }
                    catch(const std::runtime_error& e)
                    {
                        Output::LogError("Failure loading: " + asset.sourcePath.string(), "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        if(ImGui::BeginCombo("Normal##meshrenderernormalselectcombo", material.normal.c_str()))
        {
            if(ImGui::Selectable("Default"))
                meshRenderer->SetBaseColor(DefaultNormalPath);

            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                {
                    try
                    {
                        meshRenderer->SetNormal(asset.sourcePath.string());
                    }
                    catch(const std::runtime_error& e)
                    {
                        Output::LogError("Failure loading: " + asset.sourcePath.string(), "Exception: " + std::string{e.what()});
                    }
                }
            }

            ImGui::EndCombo();
        }

        if(ImGui::BeginCombo("Roughness##meshrendererroughnessselectcombo", material.roughness.c_str()))
        {
            if(ImGui::Selectable("Default"))
                meshRenderer->SetBaseColor(DefaultRoughnessPath);

            for(const auto& asset : m_assetManifest->View(AssetType::Texture))
            {
                if(ImGui::Selectable(asset.sourcePath.string().c_str()))
                {
                    try
                    {
                        meshRenderer->SetRoughness(asset.sourcePath.string());
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

    void Inspector::DrawNetworkDispatcher(NetworkDispatcher* networkDispatcher)
    {
        ElementHeader(networkDispatcher);
    }

    void Inspector::DrawParticleEmitter(ParticleEmitter* particleEmitter)
    {
        ElementHeader(particleEmitter);
    }

    void Inspector::DrawPhysicsBody(PhysicsBody* physicsBody)
    {
        ElementHeader(physicsBody);
        IMGUI_SCOPE(ItemWidth, 100.0f);
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
        IMGUI_SCOPE(ItemWidth, 100.0f);

        auto& info = pointLight->GetInfo();

        Vector3 ambient = info.ambient;
        Vector3 diffuse = info.diffuseColor;
        float diffuseIntensity = info.diffuseIntensity;

        auto ambientResult = ImGui::ColorEdit3("Ambient Color", &ambient.x, ImGuiColorEditFlags_NoInputs);
        auto diffuseResult = ImGui::ColorEdit3("Diffuse Color", &diffuse.x, ImGuiColorEditFlags_NoInputs);
        auto intensityResult = ImGui::DragFloat("Intensity", &diffuseIntensity, 1.0f, 0.0f, 600.0f, "%.2f");


        auto pointLightInfo = info;

        if (ambientResult) pointLightInfo.ambient = ambient;
        if (diffuseResult) pointLightInfo.diffuseColor = diffuse;
        if (intensityResult) pointLightInfo.diffuseIntensity = diffuseIntensity;

        if (ambientResult || diffuseResult || intensityResult)
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