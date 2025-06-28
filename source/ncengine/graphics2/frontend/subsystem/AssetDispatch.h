#pragma once

#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/CubeMapBufferResource.h"
#include "graphics2/diligent/resource/base/StructuredBuffer.h"
#include "graphics2/diligent/resource/TextureBufferResource.h"
#include "graphics2/ShaderTypes.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationStorage.h"

#include "ncengine/asset/AssetData.h"
#include "ncengine/utility/Signal.h"
#include "ncutility/NcError.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class AssetDispatch
{
    public:
        AssetDispatch(Diligent::IDeviceContext& context,
                      Diligent::IRenderDevice& device,
                      CubeMapBufferResource& cubeMapBuffer,
                      TextureBufferResource& textureBuffer,
                      TextureBufferResource& shapeKeyAnimationBuffer,
                      StructuredBuffer<ShapeKeyMetadata>& shapeKeyMetadataBuffer,
                      MeshBuffer& meshBuffer,
                      SkeletalAnimationStorage& animationStorage,
                      Signal<const asset::CubeMapUpdateEventData&>& onCubeMapEvent,
                      Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                      Signal<const asset::MeshUpdateEventData&>& onMeshEvent,
                      Signal<const asset::ShapeKeyAnimationUpdateEventData&>& onShapeKeyAnimationEvent,
                      Signal<const asset::SkeletalAnimationUpdateEventData&>& onSkeletalAnimationEvent,
                      Signal<const asset::BoneUpdateEventData&>& onBoneEvent)
            : m_context{&context},
              m_device{&device},
              m_cubeMapBuffer{&cubeMapBuffer},
              m_textureBuffer{&textureBuffer},
              m_shapeKeyAnimationBuffer{&shapeKeyAnimationBuffer},
              m_shapeKeyMetadataBuffer{&shapeKeyMetadataBuffer},
              m_meshBuffer{&meshBuffer},
              m_skeletalAnimationStorage{&animationStorage},
              m_cubeMapConnection{onCubeMapEvent.Connect(this, &AssetDispatch::OnCubeMapEvent)},
              m_textureConnection{onTextureEvent.Connect(this, &AssetDispatch::OnTextureEvent)},
              m_meshConnection{onMeshEvent.Connect(this, &AssetDispatch::OnMeshEvent)},
              m_shapeKeyAnimationConnection{onShapeKeyAnimationEvent.Connect(this, &AssetDispatch::OnShapeKeyAnimationEvent)},
              m_skeletalAnimationConnection{onSkeletalAnimationEvent.Connect(this, &AssetDispatch::OnSkeletalAnimationEvent)},
              m_boneConnection{onBoneEvent.Connect(this, &AssetDispatch::OnBoneEvent)}
        {
        }

    private:
        Diligent::IDeviceContext* m_context;
        Diligent::IRenderDevice* m_device;
        CubeMapBufferResource* m_cubeMapBuffer;
        TextureBufferResource* m_textureBuffer;
        TextureBufferResource* m_shapeKeyAnimationBuffer;
        StructuredBuffer<ShapeKeyMetadata>* m_shapeKeyMetadataBuffer;
        MeshBuffer* m_meshBuffer;
        SkeletalAnimationStorage* m_skeletalAnimationStorage;
        Connection m_cubeMapConnection;
        Connection m_textureConnection;
        Connection m_meshConnection;
        Connection m_shapeKeyAnimationConnection;
        Connection m_skeletalAnimationConnection;
        Connection m_boneConnection;

        void OnCubeMapEvent(const asset::CubeMapUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_cubeMapBuffer->Load(event.data, *m_context, *m_device);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_cubeMapBuffer->Unload();
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    throw NcError{"Unexpected UpdateAction"};
                }
            }
        }

        void OnTextureEvent(const asset::TextureUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_textureBuffer->Load<unsigned char>(event.data, *m_context, *m_device);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_textureBuffer->Unload();
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    throw NcError{"Unexpected UpdateAction"};
                }
            }
        }

        void OnMeshEvent(const asset::MeshUpdateEventData& event)
        {
            m_meshBuffer->Load(event.vertices, event.indices, *m_context, *m_device);
        }

        void OnShapeKeyAnimationEvent(const asset::ShapeKeyAnimationUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    auto animations = std::vector<nc::asset::TextureWithId<float>>{};
                    animations.reserve(event.data.size());

                    auto metadata = std::vector<ShapeKeyMetadata>{};
                    metadata.reserve(event.data.size());

                    for (const auto& animation : event.data)
                    {
                        animations.push_back(std::move(animation.animation));
                        metadata.emplace_back(0u, animation.durationInSeconds);
                    }

                    auto metadataUpdateInfo = BufferUpdateInfo<ShapeKeyMetadata>{
                        .instances = metadata,
                        .dirtyRanges = {{0, metadata.size()}}
                    };

                    m_shapeKeyAnimationBuffer->Load<float>(animations, *m_context, *m_device);
                    m_shapeKeyMetadataBuffer->Update(*m_context, *m_device, metadataUpdateInfo);
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    m_shapeKeyAnimationBuffer->Unload();
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    throw NcError{"Unexpected UpdateAction"};
                    break;
                }
            }
        }

        void OnSkeletalAnimationEvent(const asset::SkeletalAnimationUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_skeletalAnimationStorage->LoadAnimations(event.ids, event.data);
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    NC_ASSERT(event.ids.size() == 1, "Unexpected SkeletalAnimationUpdateEventData size");
                    m_skeletalAnimationStorage->UnloadAnimation(event.ids[0]);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_skeletalAnimationStorage->UnloadAllAnimations();
                    break;
                }
            }
        }

        void OnBoneEvent(const asset::BoneUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_skeletalAnimationStorage->LoadBones(event.ids, event.data);
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    NC_ASSERT(event.ids.size() == 1, "Unexpected BoneUpdateEventData size");
                    m_skeletalAnimationStorage->UnloadBones(event.ids[0]);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_skeletalAnimationStorage->UnloadAllBones();
                    break;
                }
            }
        }
};
} // namespace nc::graphics
