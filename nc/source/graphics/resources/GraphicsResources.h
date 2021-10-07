#pragma once

#include "ObjectData.h"
#include "graphics/Graphics.h"
#include "graphics/Resources/ImmutableImage.h"
#include "graphics/Resources/ImmutableBuffer.h"
#include "graphics/Resources/WriteableBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "component/PointLight.h"
#include "component/MeshRenderer.h"

#include <vector>
#include <string>

namespace nc::graphics
{
    class MeshesData
    {
        public:
            MeshesData() = default;
            MeshesData(ImmutableBuffer<Vertex> vertexBuffer, 
                       ImmutableBuffer<uint32_t> indexBuffer, 
                       std::unordered_map<std::string, Mesh> accessors);
            ~MeshesData() noexcept;
            MeshesData(MeshesData&&) = default;
            MeshesData& operator=(MeshesData&&) = default;
            MeshesData(const MeshesData&) = delete;
            MeshesData& operator=(const MeshesData&) = delete;

            const std::vector<std::string> GetMeshPaths() const;
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

    struct Texture
    {
        ImmutableImage image;
        vk::DescriptorImageInfo imageInfo;
        std::string uid;
    };

    class TexturesData
    {
        public:
            TexturesData(Graphics* graphics, uint32_t maxTexturesCount);
            ~TexturesData() noexcept;
            TexturesData(TexturesData&&) = default;
            TexturesData& operator=(TexturesData&&) = default;
            TexturesData(const TexturesData&) = delete;
            TexturesData& operator=(const TexturesData&) = delete;

            const std::vector<std::string> GetTexturePaths() const;
            bool TextureExists(const std::string& uid) const noexcept;
            vk::DescriptorSetLayout* GetDescriptorLayout() noexcept; 
            vk::DescriptorSet* GetDescriptorSet() noexcept;
            uint32_t GetAccessor(const std::string& uid) const;
            void AddTexture(Graphics* graphics, Texture texture);
            void AddTextures(Graphics* graphics, std::vector<Texture> textures);
            void Clear() noexcept;

        private:
            void UpdateTextures(Graphics* graphics);
            std::vector<Texture> m_textures;
            std::vector<vk::DescriptorImageInfo> m_imageInfos;
            std::unordered_map<std::string, uint32_t> m_accessors;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
            vk::UniqueSampler m_sampler;
            vk::ImageLayout m_layout;
            uint32_t m_maxTexturesCount;
            bool m_texturesInitialized;
    };

    class PointLightsData
    {
        public:
            PointLightsData() = default;
            PointLightsData(Graphics* graphics, uint32_t maxPointLights);
            ~PointLightsData() noexcept;
            PointLightsData(PointLightsData&&) = default;
            PointLightsData& operator=(PointLightsData&&) = default;
            PointLightsData(const PointLightsData&) = delete;
            PointLightsData& operator=(const PointLightsData&) = delete;

            void Update(const std::vector<nc::PointLightInfo>& pointLights);
            vk::DescriptorSetLayout* GetDescriptorLayout() noexcept; 
            vk::DescriptorSet* GetDescriptorSet() noexcept; 

        private:
            WriteableBuffer<nc::PointLightInfo> m_pointLightsArrayBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    };

    class ObjectsData
    {
        public:
            ObjectsData() = default;
            ~ObjectsData() noexcept;
            ObjectsData(Graphics* graphics);
            ObjectsData(ObjectsData&&) = default;
            ObjectsData& operator=(ObjectsData&&) = default;
            ObjectsData(const ObjectsData&) = delete;
            ObjectsData& operator=(const ObjectsData&) = delete;

            void Update(const std::vector<ObjectData>& objectsData);
            vk::DescriptorSetLayout* GetDescriptorLayout() noexcept; 
            vk::DescriptorSet* GetDescriptorSet() noexcept; 

        private:
            WriteableBuffer<ObjectData> m_objectsDataBuffer;
            vk::UniqueDescriptorSet m_descriptorSet;
            vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    };
}