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

#include <memory>

using namespace nc;

namespace nc
{
    /* Window stubs */
    Window::Window(HINSTANCE hi) noexcept { (void)hi; }
    Window::~Window() {}
    HWND Window::GetHWND() const noexcept { return (HWND)nullptr; }
    std::pair<int, int> Window::GetWindowDimensions() const noexcept { return m_windowDimensions; }
    void Window::ProcessSystemMessages() {}
    Window * Window::Instance = new Window((HINSTANCE)nullptr);

    /* Renderer stubs */
    Renderer::Renderer(graphics::Mesh& mesh) {(void)mesh;}
    Renderer::Renderer(Renderer&& o) {(void)o;}
    Renderer& Renderer::operator=(Renderer&& o){(void)o;return *this;}
    void Renderer::EditorGuiElement() {}
    void Renderer::SyncMaterialData(){}
    void Renderer::SetMaterial(graphics::Material& material) {(void)material;}
    void Renderer::SetMesh(graphics::Mesh& mesh) {(void)mesh;}
    void Renderer::Update(graphics::Graphics * gfx){(void)gfx;}

    /* PointLight stubs */
    PointLight::PointLight() {}
    void PointLight::EditorGuiElement() {}
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
    DirectX::XMMATRIX Graphics::GetCamera() const noexcept { return DirectX::XMMATRIX{}; }
    DirectX::XMMATRIX Graphics::GetProjection() const noexcept {return DirectX::XMMATRIX{}; }
    void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept { (void)cam; }
    void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept { (void)proj; }
    void Graphics::StartFrame() {}
    void Graphics::DrawIndexed(UINT count) { (void)count; }
    void Graphics::EndFrame() {}
    #ifdef NC_DEBUG
    uint32_t Graphics::GetDrawCallCount() const { return 0; }
    #endif

    /* Mesh stubs */
    Mesh::Mesh(std::string meshPath) {(void)meshPath;}
    MeshData& Mesh::GetMeshData() {return m_meshData;}
} //end namespace graphics

namespace utils::editor
{
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

} //end namespace utils::editor
} //end namespace nc

namespace nc
{
    class PhonyComponent : public Component
    {
    };
}

engine::Engine g_engine;

void Init()
{
    Window window((HINSTANCE)nullptr);
    NCE nce(&g_engine);
    //scene::SceneManager sceneManager;
}

void TearDown()
{
}

// void AdvanceFrame()
// {
//     g_engine.FixedUpdate();
//     g_engine.FrameLogic();
//     g_engine.FrameCleanup();
// }


TEST(EntityTest, constructorTests)
{
    Entity entity(2u, "Test");
    EXPECT_EQ(entity.Handle, 2u);
    EXPECT_EQ(entity.Tag, "Test");
}

/***********
 * NCE TESTS
 ***********/

/* CreateEntity */

/* GetEntity */
TEST(NceTest, GetEntity_exists_returnsPtr)
{
    auto eView = NCE::CreateEntity();
    auto ePtr = NCE::GetEntity(eView.Handle);
    EXPECT_NE(ePtr, nullptr);
    NCE::DestroyEntity(eView.Handle);
}
TEST(NceTest, GetEntity_doesNotExist_returnsNull)
{
    NCE::DestroyEntity(1);
    auto ePtr = NCE::GetEntity(1);
    EXPECT_EQ(ePtr, nullptr);
}
// TEST(NceTest, GetEntity_byTag_returnsPtr)
// {
//     auto handle = NCE::CreateEntity({}, {}, {}, "Tag").Handle;
//     AdvanceFrame();
//     auto actualPtr = NCE::GetEntity("Tag");
//     ASSERT_NE(actualPtr, nullptr);
//     EXPECT_EQ(handle, actualPtr->Handle);
//     NCE::DestroyEntity(handle);
// }

/* DestroyEntity */
TEST(NceTest, DestroyEntity_exists_returnsTrue)
{
    auto eView = NCE::CreateEntity();
    auto actual = NCE::DestroyEntity(eView.Handle);
    EXPECT_EQ(actual, true);
}
TEST(NceTest, DestroyEntity_doesNotExist_returnsFalse)
{
    NCE::DestroyEntity(1);
    auto actual = NCE::DestroyEntity(1);
    EXPECT_EQ(actual, false);
}

/* HasUserComponent */
TEST(NceTest, HasUserComponent_has_returnsTrue)
{
    auto eView = NCE::CreateEntity();
    NCE::AddUserComponent<PhonyComponent>(eView.Handle);
    auto actual = NCE::HasUserComponent<PhonyComponent>(eView.Handle);
    EXPECT_EQ(actual, true);
    NCE::DestroyEntity(eView.Handle);
}
TEST(NceTest,HasUserComponent_doesNotHave_returnsFalse)
{
    auto eView = NCE::CreateEntity();
    auto actual = NCE::HasUserComponent<PhonyComponent>(eView.Handle);
    EXPECT_EQ(actual, false);
    NCE::DestroyEntity(eView.Handle);
}
TEST(NceTest, HasUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::HasUserComponent<PhonyComponent>(1);
    }
    catch(const DefaultException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}

/* AddUserComponent */
TEST(NceTest, AddUserComponent_goodValues_succeeds)
{
    auto eView = NCE::CreateEntity();
    EXPECT_NE(nullptr, NCE::AddUserComponent<PhonyComponent>(eView.Handle));
    EXPECT_EQ(true, NCE::HasUserComponent<PhonyComponent>(eView.Handle));
    NCE::DestroyEntity(eView.Handle);
}
TEST(NceTest, AddUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::AddUserComponent<PhonyComponent>(1);
    }
    catch(const DefaultException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}
TEST(NceTest, AddUserComponent_doubleCall_returnsNull)
{
    auto eView = NCE::CreateEntity();
    NCE::AddUserComponent<PhonyComponent>(eView.Handle);
    auto actual = NCE::AddUserComponent<PhonyComponent>(eView.Handle);
    EXPECT_EQ(actual, nullptr);
}

/* RemoveUserComponent */
TEST(NceTest, RemoveUserComponent_hasComp_returnsTrue)
{
    auto handle = NCE::CreateEntity().Handle;
    NCE::AddUserComponent<PhonyComponent>(handle);
    auto actual = NCE::RemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, true);
    NCE::DestroyEntity(handle);
}
TEST(NceTest, RemoveUserComponent_doesNotHave_returnsFalse)
{
    auto handle = NCE::CreateEntity().Handle;
    auto actual = NCE::RemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, false);
    NCE::DestroyEntity(handle);
}
TEST(NceTest, RemoveUserComponent_badHandle_throws)
{
    NCE::DestroyEntity(1);
    auto caught = false;
    try
    {
        NCE::RemoveUserComponent<PhonyComponent>(1);
    }
    catch(const DefaultException& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}


int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    Init();
    return RUN_ALL_TESTS();
}