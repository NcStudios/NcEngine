#include "VertexDescriptions.h"

#include "ncasset/Assets.h"

namespace nc::graphics::vulkan
{
    vk::VertexInputBindingDescription GetVertexBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0);
        bindingDescription.setStride(sizeof(asset::MeshVertex));
        bindingDescription.setInputRate(vk::VertexInputRate::eVertex); // @todo Change to eInstance for instance data
        return bindingDescription;
    }

    std::array<vk::VertexInputAttributeDescription, 7> GetVertexAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 7> attributeDescriptions{};
        attributeDescriptions[0].setBinding(0);
        attributeDescriptions[0].setLocation(0);
        attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[0].setOffset(offsetof(asset::MeshVertex, position));

        attributeDescriptions[1].setBinding(0);
        attributeDescriptions[1].setLocation(1);
        attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[1].setOffset(offsetof(asset::MeshVertex, normal));

        attributeDescriptions[2].setBinding(0);
        attributeDescriptions[2].setLocation(2);
        attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
        attributeDescriptions[2].setOffset(offsetof(asset::MeshVertex, uv));

        attributeDescriptions[3].setBinding(0);
        attributeDescriptions[3].setLocation(3);
        attributeDescriptions[3].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[3].setOffset(offsetof(asset::MeshVertex, tangent));

        attributeDescriptions[4].setBinding(0);
        attributeDescriptions[4].setLocation(4);
        attributeDescriptions[4].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[4].setOffset(offsetof(asset::MeshVertex, bitangent));

        attributeDescriptions[5].setBinding(0);
        attributeDescriptions[5].setLocation(5);
        attributeDescriptions[5].setFormat(vk::Format::eR32G32B32A32Sfloat);
        attributeDescriptions[5].setOffset(offsetof(asset::MeshVertex, boneWeights));

        attributeDescriptions[6].setBinding(0);
        attributeDescriptions[6].setLocation(6);
        attributeDescriptions[6].setFormat(vk::Format::eR32G32B32A32Sint);
        attributeDescriptions[6].setOffset(offsetof(asset::MeshVertex, boneIds));
        return attributeDescriptions;
    }
} // namespace nc::graphics::Vulkan
