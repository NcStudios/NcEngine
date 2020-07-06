#include "Spawner.h"
#include "Head.h"
#include "directx/math/DirectXMath.h"
#include "Renderer.h"

using namespace nc;

Spawner::Spawner()
    : m_entities{},
      m_rng{ std::random_device{}() },
      m_angDist{ 0.0f, 3.1415f * 2.0f },
      m_posDist{ -50.0f, 50.0f },
      m_sclDist{ 0.5f, 3.0 },
      m_clrDist{ 0.0f, 1.0f },
      m_monkeyMesh{ "project\\models\\monkey.obj" },
      m_ncMesh{ "project\\models\\defaultMesh.obj" },
      m_timeSinceSpawn{ 0.0f },
      m_timeSinceDespawn{ 0.0f }
{
    for(size_t i = 0; i < 100; ++i)
    {
        Vector3  randPos{ m_posDist(m_rng), m_posDist(m_rng), m_posDist(m_rng) };
        Vector3  randRot{ };
        Vector3  randScl{ m_sclDist(m_rng), m_sclDist(m_rng), m_sclDist(m_rng) };
        DirectX::XMFLOAT3 randClr{ m_clrDist(m_rng), m_clrDist(m_rng), m_clrDist(m_rng) };
        graphics::PBRMaterial material{ };

        if (i % 2 == 0)
        {
            EntityHandle handle = NCE::CreateEntity(randPos, randRot, randScl, "Worm");
            NCE::AddUserComponent<Head>(handle);
            auto rend = NCE::AddEngineComponent<Renderer>(handle, m_ncMesh);
            rend->SetMaterial(material);
            m_entities.push_back(handle);
        }
        else
        {
            EntityHandle handle = NCE::CreateEntity(randPos, randRot, randScl, "Monkey");
            NCE::AddUserComponent<Head>(handle);
            auto rend = NCE::AddEngineComponent<Renderer>(handle, m_monkeyMesh);
            rend->SetMaterial(material);
            m_entities.push_back(handle);
        }
    }
}


void Spawner::FrameUpdate(float dt)
{
    m_timeSinceSpawn += dt;
    m_timeSinceDespawn += dt;

    if(m_timeSinceSpawn > m_spawnRate)
    {
        m_timeSinceSpawn = 0.0f;

        Vector3  randPos{ m_posDist(m_rng), m_posDist(m_rng), m_posDist(m_rng) };
        Vector3  randRot{ m_angDist(m_rng), m_angDist(m_rng), m_angDist(m_rng) };
        Vector3  randScl{ m_sclDist(m_rng), m_sclDist(m_rng), m_sclDist(m_rng) };
        DirectX::XMFLOAT3 randClr{ m_clrDist(m_rng), m_clrDist(m_rng), m_clrDist(m_rng) };
        graphics::PBRMaterial material{ };

        EntityHandle boxHandle = NCE::CreateEntity(randPos, randRot, randScl, "Worm");
        NCE::AddUserComponent<Head>(boxHandle);
        auto rend = NCE::AddEngineComponent<Renderer>(boxHandle, m_ncMesh);
        rend->SetMaterial(material);
        m_entities.push_back(boxHandle);
        return; //don't want to do despawn on recently created obj,
                //seems to crash (@todo)
    }

    if (m_timeSinceDespawn > m_despawnRate)
    {
        m_timeSinceDespawn = 0.0f;
        auto toDestroy = m_entities.back();
        m_entities.pop_back();
        NCE::DestroyEntity(toDestroy);
    }
}

