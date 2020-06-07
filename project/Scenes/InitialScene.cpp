#include "InitialScene.h"

#include <random>
#include "component/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "component/PointLight.h"

void InitialScene::Load()
{
    //add light
    EntityView lightView = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "PointLight");
    EntityView lightView2 = NCE::CreateEntity(Vector3::One(), Vector3::Zero(), Vector3::Zero(), "PointLight2");
    NCE::AddEngineComponent<PointLight>(lightView.Handle);
    NCE::AddEngineComponent<PointLight>(lightView2.Handle);

    //CamController
    EntityView camView = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "CameraController");
    NCE::AddUserComponent<CamController>(camView.Handle);

    using DirectX::XMFLOAT3; using std::mt19937; using std::uniform_real_distribution;

    mt19937 rng( std::random_device{}() );
	uniform_real_distribution<float> angDist(  0.0f,  3.1415f * 2.0f);
	uniform_real_distribution<float> posDist(-10.0f, 10.0f);
    uniform_real_distribution<float> sclDist(  0.5f,  2.0f); 
    uniform_real_distribution<float> clrDist(  0.0f,  1.0f);

    nc::graphics::Mesh cubeMesh = nc::graphics::Mesh(NCE::GetGraphics(), "project\\models\\cube_ind.obj");
    nc::graphics::Mesh monkeyMesh = nc::graphics::Mesh(NCE::GetGraphics(), "project\\models\\monkey.obj");



    // nc::graphics::Mesh cubeMesh = {};
    // cubeMesh.Name               = "CubeMesh";
    // cubeMesh.MeshPath           = "project\\models\\cube_ind.obj";
    // cubeMesh.VertexShaderPath   = "project\\shaders\\compiled\\litvertexshader.cso";
    // cubeMesh.PixelShaderPath    = "project\\shaders\\compiled\\litpixelshader.cso";
    // cubeMesh.PrimitiveTopology  = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    // cubeMesh.InputElementDesc   = 
    // {
    //     { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //     { "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    // };


    nc::graphics::Material material = {};
    XMFLOAT3 redColor (255, 0, 0);
    material.color = redColor;


    // nc::graphics::Mesh monkeyMesh = cubeMesh;
    // monkeyMesh.Name               = "MonkeyMesh";
    // monkeyMesh.MeshPath           = "project\\models\\monkey.obj";

    for(int i = 0; i < 10; ++i)
    {
        Vector3  randPos (posDist(rng), posDist(rng), posDist(rng));
        Vector3  randRot (        0.0f,         0.0f,         0.0f);
        Vector3  randScl (sclDist(rng), sclDist(rng), sclDist(rng));
        XMFLOAT3 randClr (clrDist(rng), clrDist(rng), clrDist(rng));

        if((i%2) == 0)
        {
            EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Box");
            NCE::AddUserComponent<Head>(boxView.Handle);
            NCE::AddEngineComponent<Renderer>(boxView.Handle)->SetMesh(NCE::GetGraphics(), cubeMesh);
        }
        else
        {
            EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Monkey");
            NCE::AddUserComponent<Head>(boxView.Handle);
            //NCE::AddEngineComponent<Renderer>(boxView.Handle)->SetMesh(NCE::GetGraphics(), monkeyMesh);
            NCE::AddEngineComponent<Renderer>(boxView.Handle)->SetMaterial(NCE::GetGraphics(), material);
        }
    }
}
 
void InitialScene::Unload()
{
    
}