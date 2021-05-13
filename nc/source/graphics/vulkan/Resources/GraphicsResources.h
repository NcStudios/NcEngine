#pragma once

#include "graphics/vulkan/Resources/ImmutableImage.h"
#include "graphics/vulkan/Resources/ImmutableBuffer.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace nc::graphics::vulkan
{
    struct MeshesData
    {
        ImmutableBuffer<Vertex> vertexBuffer;
        ImmutableBuffer<uint32_t> indexBuffer;
        std::unordered_map<std::string, Mesh> accessors;
    };

    struct TexturesData
    {
        // Each texture is represented here
        std::vector<vulkan::ImmutableImage> textureBuffers;
        std::vector<vk::DescriptorImageInfo> imageInfos;
        std::unordered_map<std::string, uint32_t> accessors;

        // Only need one of the below for all textures
        vk::DescriptorSet descriptorSet;
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::Sampler sampler;
        vk::ImageLayout layout;
    };

    struct GraphicsResources
    {
        MeshesData meshes;
        TexturesData textures;
    };
}