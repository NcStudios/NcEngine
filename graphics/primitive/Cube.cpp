#include "Cube.h"
#include "../internal/BindablePipeline.h"
#include "../internal/DXException.h"

namespace nc::graphics::primitive
{

	using namespace nc::graphics::internal;

Cube::Cube(Graphics& graphics, std::mt19937& rng,
         std::uniform_real_distribution<float>& adist,
         std::uniform_real_distribution<float>& ddist,
         std::uniform_real_distribution<float>& odist,
         std::uniform_real_distribution<float>& rdist )
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
		struct Vertex
		{
			struct{ float x, y, z; } pos;
		};

		const std::vector<Vertex> vertices =
		{
			{-1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
		};

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, vertices));

		auto pvs = std::make_unique<VertexShader>(graphics, L"graphics\\shader\\compiled\\vertexshader.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>(graphics, L"graphics\\shader\\compiled\\pixelshader.cso"));

		const std::vector<unsigned short> indices =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, indices));

		struct ConstantBuffer2
		{
			struct { float r, g, b, a; } face_colors[6];
		};

		const ConstantBuffer2 cb2 =
		{
			{
				{ 1.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 0.0f, 1.0f, 1.0f },
			}
		};

		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(graphics, cb2));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	} //end if !IsStaticInitialized()
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));
}

void Cube::Update(float dt) noexcept
{
	roll  += droll  * dt;
	pitch += dpitch * dt;
	yaw   += dyaw   * dt;
	theta += dtheta * dt;
	phi   += dphi   * dt;
	chi   += dchi   * dt;
}

DirectX::XMMATRIX Cube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		   DirectX::XMMatrixTranslation( r,0.0f,0.0f )             *
		   DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi )  *
		   DirectX::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}

}// end namespace nc::graphics::internal