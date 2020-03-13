#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <d3d11.h>
#include "DirectXMath.H"

#include "DrawableBase.h"
#include "Mesh.h"
#include "MaterialType.h"
#include "ObjLoader.h"
#include "NCVector.h"
#include "Transform.h"

namespace nc::graphics
{
    struct PSMaterialConstants
    {
        DirectX::XMFLOAT3 color;
        float specularIntensity = 0.6;
        float specularPower = 30.0f;
        float padding[3];
    };

    using namespace nc::graphics::d3dresource;

    template<typename T>
    class Model : public DrawableBase<T> 
    {
        public:
            //Mesh mesh;
            DirectX::XMMATRIX transformationMatrix;

            //material
            PSMaterialConstants m_materialData;

            Model(Graphics& graphics, DirectX::XMFLOAT3 materialColor)
            {
                if(!DrawableBase<T>::IsStaticInitialized())
                {
                    utils::ObjLoader loader;
                    T mesh;
                    loader.Load(&mesh);
                    
                    DrawableBase<T>::AddStaticBind(std::make_unique<VertexBuffer>(graphics, mesh.Vertices));
                    auto pvs = std::make_unique<VertexShader>(graphics, mesh.GetVertexShaderPath());
                    auto pvsbc = pvs->GetBytecode();
                    DrawableBase<T>::AddStaticBind(std::move(pvs));
                    DrawableBase<T>::AddStaticBind(std::make_unique<PixelShader>(graphics, mesh.GetPixelShaderPath()));
                    DrawableBase<T>::AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, mesh.Indices));
                    DrawableBase<T>::AddStaticBind(std::make_unique<InputLayout>(graphics, mesh.GetIED(), pvsbc));
                    DrawableBase<T>::AddStaticBind(std::make_unique<Topology>(graphics, mesh.GetTopology()));
                }
                else
                {
                    DrawableBase<T>::SetIndexFromStatic();
                }

                Drawable::AddBind(std::make_unique<TransformCbuf>(graphics, *this));
                m_materialData.color = materialColor;
                Drawable::AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstants>>(graphics, m_materialData, 1u));
            }

            void UpdateTransformationMatrix(Transform* transform) noexcept override
            {
                // Vector3 pos = transform->GetPosition();
                // Vector3 rot = transform->GetRotation();
                // Vector3 scl = transform->GetScale();                

                // transformationMatrix = DirectX::XMMatrixScaling(scl.GetX(), scl.GetY(), scl.GetZ())              *
                //                        DirectX::XMMatrixRotationRollPitchYaw(rot.GetX(), rot.GetY(), rot.GetZ()) *
                //                        DirectX::XMMatrixTranslation( pos.GetX(), pos.GetY(), pos.GetZ() );//             *
                //                        //DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi );  

                
                //transformationMatrix = transform->CamGetMatrix();
                transformationMatrix = transform->GetMatrixXM();
            }

            DirectX::XMMATRIX GetTransformXM() const noexcept override
            {
                return transformationMatrix;
            }

            PSMaterialConstants* GetMaterial() noexcept override
            {
                return &m_materialData;
            }

            
    };
}