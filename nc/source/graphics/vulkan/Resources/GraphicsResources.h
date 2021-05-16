#pragma once

#include "graphics/vulkan/Resources/ImmutableImage.h"
#include "graphics/vulkan/Resources/ImmutableBuffer.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"
#include "component/vulkan/PointLight.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace nc::graphics::vulkan
{
    class MeshesData
    {
        public:
            MeshesData() = default;
            MeshesData(ImmutableBuffer<Vertex> vertexBuffer, 
                       ImmutableBuffer<uint32_t> indexBuffer, 
                       std::unordered_map<std::string, Mesh> accessors);
                       
            bool MeshExists(const std::string& uid) const noexcept;
            vk::Buffer* GetVertexBuffer();
            vk::Buffer* GetIndexBuffer();
            const Mesh& GetAccessor(const std::string& uid) const;
            void Clear() noexcept;

        private:
            ImmutableBuffer<Vertex> m_vertexBuffer;
            ImmutableBuffer<uint32_t> m_indexBuffer;
            std::unordered_map<std::string, Mesh> m_accessors;
    };

    class TexturesData
    {
        public:
            TexturesData() = default;
            TexturesData(std::vector<vulkan::ImmutableImage> textureBuffers, 
                         std::vector<vk::DescriptorImageInfo> imageInfos, 
                         std::unordered_map<std::string, uint32_t> accessors,
                         vk::UniqueDescriptorSet descriptorSet,
                         vk::UniqueDescriptorSetLayout descriptorSetLayout,
                         vk::UniqueSampler sampler,
                         vk::ImageLayout layout);

            bool TextureExists(const std::string& uid) const noexcept;
            vk::DescriptorSetLayout* GetDescriptorLayout() noexcept; 
            vk::DescriptorSet* GetDescriptorSet() noexcept;
            uint32_t GetAccessor(const std::string& uid) const;
            void Clear() noexcept;

        private:
            // Each texture is represented here
            std::vector<vulkan::ImmutableImage> m_textureBuffers;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            std::unordered_map<std::string, uint32_t> m_accessors;

            // Only need one of the below for all textures
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            vk::UniqueSampler m_sampler;
            vk::ImageLayout m_layout;
    };

    class PointLightsData
    {
        public:
            PointLightsData() = default;
            PointLightsData();

            void Add(PointLightInfo& info);

            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    };
}