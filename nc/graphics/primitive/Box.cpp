#include "Box.h"
#include "BindablePipeline.h"
#include "DXException.h"


#include "DirectXMath.h"
#include "Cube.h"
#include "EditorManager.h"

#include "Graphics.h"

#include "ObjLoader.h"
#include "Model.h"

namespace nc::graphics::primitive
{

	using namespace nc::graphics::d3dresource;

Box::Box(Graphics& graphics, std::mt19937& rng,
         std::uniform_real_distribution<float>& adist,
         std::uniform_real_distribution<float>& ddist,
         std::uniform_real_distribution<float>& odist,
         std::uniform_real_distribution<float>& rdist,
		 DirectX::XMFLOAT3 materialColor)
	: r(rdist(rng)),
	  theta(adist(rng)),
	  phi(adist(rng)),
	  chi(adist(rng)),
	  droll(ddist(rng)),
	  dpitch(ddist( rng)),
	  dyaw(ddist(rng)),
	  dtheta(odist(rng)),
	  dphi(odist(rng)),
	  dchi(odist(rng))
{
	if(!IsStaticInitialized())
	{
		//define vertex type
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		//get cube vertices and indices
		//auto model = Cube::MakeIndependent<Vertex>();
		//model.SetNormalsIndependentFlat();

		utils::ObjLoader loader;
		Model model = loader.Load("nc\\utils\\objloader\\testobj.obj");
		//model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices));

		auto pvs = std::make_unique<VertexShader>(graphics, model.vertShaderPath);
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, model.pixShaderPath));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));

		AddStaticBind(std::make_unique<InputLayout>(graphics, model.ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, model.topology));
	} //end if !IsStaticInitialized()
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));

	m_materialData.color = materialColor;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstants>>(graphics, m_materialData, 1u));
}


template<typename T>
T wrap_angle(T theta)
{
	const auto PI = 3.14159265358979;
	const T t = fmod(theta, (T)2.0 * (T)PI);
	return (t > (T)PI ? (t - (T)2.0 * (T)PI) : t);
}


void Box::Update(float dt) noexcept
{
	roll  = wrap_angle(roll  + droll  * dt);
	pitch = wrap_angle(pitch + dpitch * dt);
	yaw   = wrap_angle(yaw   + dyaw   * dt);
	theta = wrap_angle(theta + dtheta * dt);
	phi   = wrap_angle(phi   + dphi   * dt);
	chi   = wrap_angle(chi   + dchi   * dt);
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		   DirectX::XMMatrixTranslation( r,0.0f,0.0f )             *
		   DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi );
}

void Box::SpawnControlWindow(int id, Graphics& graphics)
{
	//bool open = true;
	bool needSync = utils::editor::EditorManager::BoxControl(id, &m_materialData.color.x, &m_materialData.specularIntensity, &m_materialData.specularPower,
	                                                         &r, &theta, &phi);
	if(needSync)
	{
		SyncMaterialData(graphics);
	}
	
}

void Box::SyncMaterialData(Graphics& graphics)
{
	auto pConstPS = QueryBindable<PixelConstantBuffer<PSMaterialConstants>>();
	assert(pConstPS != nullptr);
	pConstPS->Update(graphics, m_materialData);
}

}// end namespace nc::graphics::internal