#include "Box2.h"
#include "BindablePipeline.h"
#include "DXException.h"


#include "DirectXMath.h"
#include "Cube.h"

namespace nc::graphics::primitive
{

	using namespace nc::graphics::internal;

Box2::Box2(Graphics& graphics)
{
	if(!IsStaticInitialized())
	{
		//define vertex type
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		//get cube vertices and indices
		auto model = Cube::Make<Vertex>();

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices));

		auto pvs = std::make_unique<VertexShader>(graphics, L"graphics\\shader\\compiled\\vertexshader.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"graphics\\shader\\compiled\\pixelshader.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));

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

void Box2::Update(float dt) noexcept
{

}

DirectX::XMMATRIX Box2::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(0.0f ,0.0f, 0.0f) *
		   DirectX::XMMatrixTranslation( 0.0f ,0.0f, 0.0f );
}

}// end namespace nc::graphics::internal