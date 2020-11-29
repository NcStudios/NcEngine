#include "gtest/gtest.h"
#include "component/Component.h"
#include "NcCommon.h"
#include "NcEngine.h"

/** stubbed */
#include "graphics/Graphics.h"
#include "Window.h"
#include "engine/system/RenderingSystem.h"
#include "engine/system/ComponentSystem.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "ui/UI.h"
#include "ui/Editor.h"
#include "config/Config.h"

#include <memory>

using namespace nc;

namespace nc
{
    /* Window stubs */
    Window::Window(HINSTANCE hi, const config::Config& config) { (void)hi;(void)config; }
    Window::~Window() {}
    HWND Window::GetHWND() const noexcept { return (HWND)nullptr; }
    std::pair<int, int> Window::GetWindowDimensions() const noexcept { return {0,0}; }
    void Window::ProcessSystemMessages() {}
    #ifdef NC_EDITOR_ENABLED
    void Window::BindUI(ui::UI * ui){(void)ui;}
    #endif
    Window * Window::Instance = new Window((HINSTANCE)nullptr, {});

    /* Renderer stubs */
    Renderer::Renderer() = default;
    Renderer::Renderer(graphics::Mesh& mesh, graphics::Material& material){ (void)mesh;(void)material; }
    Renderer::Renderer(Renderer&& o) {(void)o;}
    Renderer& Renderer::operator=(Renderer&& o){(void)o;return *this;}
    #ifdef NC_EDITOR_ENABLED
    void Renderer::EditorGuiElement() {}
    void Renderer::SyncMaterialData() {}
    #endif
    void Renderer::SetMaterial(graphics::Material& material) {(void)material;}
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

namespace engine::system
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

namespace ui::editor
{
    #ifdef NC_EDITOR_ENABLED
    Editor::Editor(nc::graphics::Graphics * graphics) {(void)graphics;}
    void Editor::Frame(float* dt, float frameLogicTime, std::unordered_map<EntityHandle, Entity>& activeEntities) {(void)dt;(void)frameLogicTime;(void)activeEntities;}
    void Editor::ToggleGui() noexcept {}
    void Editor::EnableGui() noexcept {}
    void Editor::DisableGui() noexcept {}
    bool Editor::IsGuiActive() const noexcept { return false; }
    #endif

} //end namespace ui::editor
} //end namespace nc

namespace nc
{
    class PhonyComponent : public Component
    {
    };
}


void InitEngine()
{
    const auto configPaths = nc::config::detail::ConfigPaths
    {
        "project/config/project.ini",
        "project/config/graphics.ini",
        "project/config/physics.ini"
    };

    nc::engine::NcInitializeEngine(nullptr, std::move(configPaths));
}


/***********
 * NCE TESTS
 ***********/
TEST(Nce, GetEntity_exists_returnsPtr)
{
    auto handle = NcCreateEntity();
    auto ptr = NcGetEntity(handle);
    EXPECT_NE(ptr, nullptr);
    // NcDestroyEntity(handle);
}
TEST(Nce, GetEntity_doesNotExist_returnsNull)
{
    NcDestroyEntity(1);
    auto ePtr = NcGetEntity(1);
    EXPECT_EQ(ePtr, nullptr);
}
TEST(Nce, DestroyEntity_exists_returnsTrue)
{
    auto handle = NcCreateEntity();
    auto actual = NcDestroyEntity(handle);
    EXPECT_EQ(actual, true);
}
TEST(Nce, DestroyEntity_doesNotExist_returnsFalse)
{
    NcDestroyEntity(1);
    auto actual = NcDestroyEntity(1);
    EXPECT_EQ(actual, false);
}
TEST(Nce, HasUserComponent_has_returnsTrue)
{
    auto handle = NcCreateEntity();
    NcAddUserComponent<PhonyComponent>(handle);
    auto actual = NcHasUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, true);
    NcDestroyEntity(handle);
}
TEST(Nce,HasUserComponent_doesNotHave_returnsFalse)
{
    auto handle = NcCreateEntity();
    auto actual = NcHasUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, false);
    NcDestroyEntity(handle);
}
TEST(Nce, HasUserComponent_badHandle_throws)
{
    NcDestroyEntity(1);
    auto caught = false;
    try
    {
        NcHasUserComponent<PhonyComponent>(1);
    }
    catch(const std::runtime_error& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}
TEST(Nce, AddUserComponent_goodValues_succeeds)
{
    auto handle = NcCreateEntity();
    EXPECT_NE(nullptr, NcAddUserComponent<PhonyComponent>(handle));
    EXPECT_EQ(true, NcHasUserComponent<PhonyComponent>(handle));
    NcDestroyEntity(handle);
}
TEST(Nce, AddUserComponent_badHandle_throws)
{
    NcDestroyEntity(1);
    auto caught = false;
    try
    {
        NcAddUserComponent<PhonyComponent>(1);
    }
    catch(const std::runtime_error& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}
TEST(Nce, AddUserComponent_doubleCall_returnsNull)
{
    auto handle = NcCreateEntity();
    NcAddUserComponent<PhonyComponent>(handle);
    auto actual = NcAddUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, nullptr);
}
TEST(Nce, RemoveUserComponent_hasComp_returnsTrue)
{
    auto handle = NcCreateEntity();
    NcAddUserComponent<PhonyComponent>(handle);
    auto actual = NcRemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, true);
    NcDestroyEntity(handle);
}
TEST(Nce, RemoveUserComponent_doesNotHave_returnsFalse)
{
    auto handle = NcCreateEntity();
    auto actual = NcRemoveUserComponent<PhonyComponent>(handle);
    EXPECT_EQ(actual, false);
    NcDestroyEntity(handle);
}
TEST(Nce, RemoveUserComponent_badHandle_throws)
{
    NcDestroyEntity(1);
    auto caught = false;
    try
    {
        NcRemoveUserComponent<PhonyComponent>(1);
    }
    catch(const std::runtime_error& e)
    {
        caught = true;
    }
    EXPECT_EQ(caught, true);
}


int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    InitEngine();
    return RUN_ALL_TESTS();
}