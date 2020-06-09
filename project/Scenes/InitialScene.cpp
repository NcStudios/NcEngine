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
    NCE::AddEngineComponent<PointLight>(lightView.Handle);

    //CamController
    EntityView camView = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "CameraController");
    NCE::AddUserComponent<CamController>(camView.Handle);

    using DirectX::XMFLOAT3; using std::mt19937; using std::uniform_real_distribution;

    mt19937 rng( std::random_device{}() );
	uniform_real_distribution<float> angDist(  0.0f,  3.1415f * 2.0f);
	uniform_real_distribution<float> posDist(-10.0f, 10.0f);
    uniform_real_distribution<float> sclDist(  0.5f,  2.0f); 
    uniform_real_distribution<float> clrDist(  0.0f,  1.0f);

    // Create meshes to be added to the entities' Renderer component
    nc::graphics::Mesh monkeyMesh = nc::graphics::Mesh("project\\models\\monkey.obj");
    nc::graphics::Mesh ncMesh = nc::graphics::Mesh("project\\models\\defaultMesh.obj");

    // Create materials to be added to the entities' Renderer component
    //nc::graphics::Material monkeyMaterial = nc::graphics::Material()

    for(int i = 0; i < 10; ++i)
    {
        Vector3  randPos (posDist(rng), posDist(rng), posDist(rng));
        Vector3  randRot (        0.0f,         0.0f,         0.0f);
        Vector3  randScl (sclDist(rng), sclDist(rng), sclDist(rng));
        XMFLOAT3 randClr (clrDist(rng), clrDist(rng), clrDist(rng));

        nc::graphics::Material material = {};
        material.color = randClr;

        if((i%2) == 0)
        {
            EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Worm");
            NCE::AddUserComponent<Head>(boxView.Handle);
            NCE::AddEngineComponent<Renderer>(boxView.Handle, ncMesh);
        }
        else
        {
            EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Monkey");
            NCE::AddUserComponent<Head>(boxView.Handle);
            NCE::AddEngineComponent<Renderer>(boxView.Handle, monkeyMesh)->SetMaterial(material);
        }
    }
}
 
void InitialScene::Unload()
{
    
}