#pragma once

#include <random>
#include "scene/Scene.h"
#include "NCE.h"
#include "Head.h"
#include "CamController.h"

#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

class InitialScene : public nc::scene::Scene
{
    public:
        void Load() override;
        void Unload() override;

    private:
        using VertexType = DirectX::VertexPositionColor;
        std::unique_ptr<DirectX::CommonStates> m_states;
        std::unique_ptr<DirectX::BasicEffect> m_effect;
        std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

};