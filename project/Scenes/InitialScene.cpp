#include "InitialScene.h"

#include <random>
#include "Renderer.h"
#include "MeshTypeMonkey.h"
#include "MeshTypeCube.h"


void InitialScene::Load()
{
    //CamController
    EntityView camView = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "CameraController");
    camView.Entity()->AddComponent<CamController>();


    using DirectX::XMFLOAT3; using std::mt19937; using std::uniform_real_distribution;

    mt19937 rng( std::random_device{}() );
	uniform_real_distribution<float> angDist(0.0f,3.1415f * 2.0f);
	uniform_real_distribution<float> posDist(-10.0f,10.0f);
    uniform_real_distribution<float> sclDist(0.5f, 2.0f); 
    uniform_real_distribution<float> clrDist(0.0f, 1.0f);

    //Monkeys
    for(int i = 0; i < 10; ++i)
    {
        Vector3  randPos (posDist(rng), posDist(rng), posDist(rng));
        Vector3  randRot (0.0f,         0.0f,         0.0f);
        Vector3  randScl (sclDist(rng), sclDist(rng), sclDist(rng));
        XMFLOAT3 randClr (clrDist(rng), clrDist(rng), clrDist(rng));

        EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Monkey");
        boxView.Entity()->AddComponent<Head>();
        boxView.AddRenderer()->SetModel<MeshTypeMonkey>(NCE::GetGraphics(), randClr);
    }

    //Boxes
    for(int i = 0; i < 10; ++i)
    {
        Vector3  randPos (posDist(rng), posDist(rng), posDist(rng));
        Vector3  randRot (0.0f,         0.0f,         0.0f);
        Vector3  randScl (sclDist(rng), sclDist(rng), sclDist(rng));
        XMFLOAT3 randClr (clrDist(rng), clrDist(rng), clrDist(rng));

        EntityView boxView = NCE::CreateEntity(randPos, randRot, randScl, "Box");
        boxView.Entity()->AddComponent<Head>();
        boxView.AddRenderer()->SetModel<MeshTypeCube>(NCE::GetGraphics(), randClr);
    }

    
}
 
void InitialScene::Unload()
{
    
}