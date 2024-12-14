#pragma once

#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/TextureBufferResource.h"
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
                      TextureBufferResource& textureBuffer,
                      MeshBuffer& meshBuffer,
                      SkeletalAnimationStorage& animationStorage,
                      Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                      Signal<const asset::MeshUpdateEventData&>& onMeshEvent,
                      Signal<const asset::SkeletalAnimationUpdateEventData&>& onAnimationEvent,
                      Signal<const asset::BoneUpdateEventData&>& onBoneEvent)
            : m_context{&context},
              m_device{&device},
              m_textureBuffer{&textureBuffer},
              m_meshBuffer{&meshBuffer},
              m_animationStorage{&animationStorage},
              m_textureConnection{onTextureEvent.Connect(this, &AssetDispatch::OnTextureEvent)},
              m_meshConnection{onMeshEvent.Connect(this, &AssetDispatch::OnMeshEvent)},
              m_animationConnection{onAnimationEvent.Connect(this, &AssetDispatch::OnAnimationEvent)},
              m_boneConnection{onBoneEvent.Connect(this, &AssetDispatch::OnBoneEvent)}
        {
        }

    private:
        Diligent::IDeviceContext* m_context;
        Diligent::IRenderDevice* m_device;
        TextureBufferResource* m_textureBuffer;
        MeshBuffer* m_meshBuffer;
        SkeletalAnimationStorage* m_animationStorage;
        Connection m_textureConnection;
        Connection m_meshConnection;
        Connection m_animationConnection;
        Connection m_boneConnection;

        void OnTextureEvent(const asset::TextureUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_textureBuffer->Load(event.data, *m_context, *m_device);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_textureBuffer->Unload();
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    throw "unhandled action";
                }
            }
        }

        void OnMeshEvent(const asset::MeshUpdateEventData& event)
        {
            m_meshBuffer->Load(event.vertices, event.indices, *m_context, *m_device);
        }

        void OnAnimationEvent(const asset::SkeletalAnimationUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_animationStorage->LoadAnimations(event.ids, event.data);
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    NC_ASSERT(event.ids.size() == 1, "Unexpected SkeletalAnimationUpdateEventData size");
                    m_animationStorage->UnloadAnimation(event.ids[0]);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_animationStorage->UnloadAllAnimations();
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
                    m_animationStorage->LoadBones(event.ids, event.data);
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    NC_ASSERT(event.ids.size() == 1, "Unexpected BoneUpdateEventData size");
                    m_animationStorage->UnloadBones(event.ids[0]);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_animationStorage->UnloadAllBones();
                    break;
                }
            }
        }
};
} // namespace nc::graphics
