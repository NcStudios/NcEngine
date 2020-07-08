#include "gtest/gtest.h"
#include "NCE.h"
#include "Component.h"
#include "engine/Engine.h"
#include "Entity.h"

/** stubbed */
#include "graphics/Graphics.h"
#include "win32/Window.h"
#include "engine/RenderingSystem.h"
#include "engine/ComponentSystem.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "utils/editor/EditorManager.h"
#include "config/Config.h"

#include <memory>

using namespace nc;

namespace nc
{
    /* Window stubs */
    Window::Window(HINSTANCE hi, const config::Config& config) { (void)hi;(void)config; }
    Window::~Window() {}
    HWND Window::GetHWND() const noexcept { return (HWND)nullptr; }
    std::pair<int, int> Window::GetWindowDimensions() const noexcept { return m_windowDimensions; }
    void Window::ProcessSystemMessages() {}
    Window * Window::Instance = new Window((HINSTANCE)nullptr, {});

    /* Renderer stubs */
    Renderer::Renderer() = default;
    Renderer::Renderer(graphics::Mesh& mesh) {(void)mesh;}
    Renderer::Renderer(Renderer&& o) {(void)o;}
    Renderer& Renderer::operator=(Renderer&& o){(void)o;return *this;}
    #ifdef NC_EDITOR_ENABLED
    // void Renderer::EditorGuiElement() {}
    // void Renderer::SyncMaterialData(){}
    #endif
    void Renderer::SetMaterial(graphics::PBRMaterial& material) {(void)material;}
    void Renderer::SetMesh(graphics::Mesh& mesh) {(void)mesh;}
    void Renderer::Update(graphics::Graphics * gfx){(void)gfx;}

    /* PointLight stubs */
    PointLight::PointLight() {}
    PointLight::~PointLight() = default;
    PointLight::PointLight(PointLight&&) {} //don't need actual impl on these?
    PointLight& PointLight::operator=(PointLight&&) {return *this;}
    #ifdef NC_EDITOR_ENABLED
    void PointLight::EditorGuiElement() {}
    #endif
    void PointLight::Set(DirectX::XMFLOAT3 pos, float radius) {(void)pos;(void)radius;}
    void PointLight::Bind(DirectX::FXMMATRIX view) noexcept(false) {(void)view;}

namespace engine
{
    /* RenderingSystem stubs */
    RenderingSystem::RenderingSystem(int a, int b, HWND c) {(void)a;(void)b;(void)c;}
    RenderingSystem::~RenderingSystem(){}
    void RenderingSystem::FrameBegin() {};
    void RenderingSystem::Frame() {};
    void RenderingSystem::FrameEnd() {};
    graphics::Graphics * RenderingSystem::GetGraphics() { return nullptr; }
} //end namespace engine 

namespace graphics
{
    /* Graphics stubs */
    Graphics::Graphics(HWND hwnd, float screenWidth, float screenHeight){ (void)hwnd; (void)screenWidth; (void)screenHeight; }
    Graphics::~Graphics() = default;
    DirectX::XMMATRIX Graphics::GetCamera() const noexcept { return DirectX::XMMATRIX{}; }
    DirectX::XMMATRIX Graphics::GetProjection() const noexcept {return DirectX::XMMATRIX{}; }
    void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept { (void)cam; }
    void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept { (void)proj; }
    void Graphics::StartFrame() {}
    void Graphics::DrawIndexed(UINT count) { (void)count; }
    void Graphics::EndFrame() {}
    #ifdef NC_EDITOR_ENABLED
    uint32_t Graphics::GetDrawCallCount() const { return 0; }
    #endif

    /* Mesh stubs */
    Mesh::Mesh(std::string meshPath) {(void)meshPath;}
    MeshData& Mesh::GetMeshData() {return m_meshData;}
} //end namespace graphics

namespace utils::editor
{
    #ifdef NC_EDITOR_ENABLED
    EditorManager::EditorManager(HWND hwnd, nc::graphics::Graphics * graphics) {(void)hwnd;(void)graphics;}
    EditorManager::~EditorManager() noexcept {}

    LRESULT EditorManager::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {(void)hwnd;(void)message;(void)wParam;(void)lParam; return LRESULT{}; }

    void EditorManager::BeginFrame() {}
    void EditorManager::Frame(float* dt, float frameLogicTime, std::unordered_map<EntityHandle, Entity>& activeEntities) {(void)dt;(void)frameLogicTime;(void)activeEntities;}
    void EditorManager::EndFrame() {}

    void EditorManager::ToggleGui() noexcept {}
    void EditorManager::EnableGui() noexcept {}
    void EditorManager::DisableGui() noexcept {}
    bool EditorManager::IsGuiActive() const noexcept { return false; }
    #endif

} //end namespace utils::editor
} //end namespace nc

namespace nc
{
    class PhonyComponent : public Component
    {
    };
}



TEST(EntityTest, constructorTests)
{
    Entity entity(2u, 0u, "Test");
    EXPECT_EQ(entity.Handle, 2u);
    EXPECT_EQ(entity.Tag, "Test");
    EXPECT_EQ(entity.Handles.transform, 0u);
}

/***********
 * NCE TESTS
 ***********/
TEST(Nce, GetEntity_exists_returnsPtr)
{
    // auto handle = NCE::CreateEntity();
    // auto ptr = NCE::GetEntity(handle);
    // EXPECT_NE(ptr, nullptr);
    //NCE::DestroyEntity(handle);
}
TEST(Nce, GetEntity_doesNotExist_returnsNull)
{
    NCE::DestroyEntity(1);
    auto ePtr = NCE::GetEntity(1);
    EXPECT_EQ(ePtr, nullptr);
}
TEST(Nce, DestroyEntity_exists_returnsTrue)
{
    auto handle = NCE::CreateEntity();
    auto actual = NCE::DestroyEntity(handle);
    EXPECT_EQ(actual, true);
}
TEST(Nce, DestroyEntity_doesNotExist_returnsFalse)
{
    NCE::DestroyEntity(1);
    auto actual = NCE::DestroyEntity(1);
    EXPECT_EQ(actual, false);
}
TEST(Nce, HasUserComponent_has_returnsTrue)
{
    auto handle = NCE::CreateEntity();
    NCE::AddUserComponent<PhonyComponent>(handle);
    auto actual = NCE::HasUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, true);
    NCE::DestroyEntity(handle);
}
TEST(Nce,HasUserComponent_doesNotHave_returnsFalse)
{
    auto handle = NCE::CreateEntity();
    auto actual = NCE::HasUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, false);
    NCE::DestroyEntity(handle);
}
TEST(Nce, HasUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::HasUserComponent<PhonyComponent>(1);
    }
    catch(const NcException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}
TEST(Nce, AddUserComponent_goodValues_succeeds)
{
    auto handle = NCE::CreateEntity();
    EXPECT_NE(nullptr, NCE::AddUserComponent<PhonyComponent>(handle));
    EXPECT_EQ(true, NCE::HasUserComponent<PhonyComponent>(handle));
    NCE::DestroyEntity(handle);
}
TEST(Nce, AddUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::AddUserComponent<PhonyComponent>(1);
    }
    catch(const NcException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}
TEST(Nce, AddUserComponent_doubleCall_returnsNull)
{
    auto handle = NCE::CreateEntity();
    NCE::AddUserComponent<PhonyComponent>(handle);
    auto actual = NCE::AddUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, nullptr);
}
TEST(Nce, RemoveUserComponent_hasComp_returnsTrue)
{
    auto handle = NCE::CreateEntity();
    NCE::AddUserComponent<PhonyComponent>(handle);
    auto actual = NCE::RemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, true);
    NCE::DestroyEntity(handle);
}
TEST(Nce, RemoveUserComponent_doesNotHave_returnsFalse)
{
    auto handle = NCE::CreateEntity();
    auto actual = NCE::RemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, false);
    NCE::DestroyEntity(handle);
}
TEST(Nce, RemoveUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::RemoveUserComponent<PhonyComponent>(1);
    }
    catch(const NcException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}


int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}