#include "Box.h"
#include "BindablePipeline.h"
#include "DXException.h"


#include "DirectXMath.h"
#include "Cube.h"

namespace nc::graphics::primitive
{

	using namespace nc::graphics::internal;

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
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices));

		auto pvs = std::make_unique<VertexShader>(graphics, L"graphics\\shader\\compiled\\litvertexshader.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"graphics\\shader\\compiled\\litpixelshader.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	} //end if !IsStaticInitialized()
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));

	struct PSMaterialConstant
	{
		alignas(16)DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6;
		float specularPower = 30.0f;
		float padding[2];
	} colorConst;

	colorConst.color = materialColor;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphics, colorConst, 1u));
}

void Box::Update(float dt) noexcept
{
	roll  += droll  * dt;
	pitch += dpitch * dt;
	yaw   += dyaw   * dt;
	theta += dtheta * dt;
	phi   += dphi   * dt;
	chi   += dchi   * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		   DirectX::XMMatrixTranslation( r,0.0f,0.0f )             *
		   DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi );
}

}// end namespace nc::graphics::internal