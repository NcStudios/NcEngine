#pragma once
#include "graphics/vulkan/resources/GraphicsResources.h"

namespace nc::graphics::vulkan
{
    class ResourceManager
    {
        public:
            static void AddTextures(TexturesData textures);
            static bool TextureExists(const std::string& uid);
            static uint32_t GetTextureAccessor(const std::string& uid);
            static vk::DescriptorSet* GetDescriptorSet();
            static vk::DescriptorSetLayout* GetDescriptorSetLayout();

            static void AddMeshes(MeshesData meshes);
            static bool MeshExists(const std::string& uid);
            static const Mesh& GetMeshAccessor(const std::string& uid);
            static vk::Buffer* GetVertexBuffer();
            static vk::Buffer* GetIndexBuffer();

        private:
            void AddTextures_(TexturesData textures);
            bool TextureExists_(const std::string& uid);
            uint32_t GetTextureAccessor_(const std::string& uid);
            vk::DescriptorSet* GetDescriptorSet_();
            vk::DescriptorSetLayout* GetDescriptorSetLayout_();

            void AddMeshes_(MeshesData meshes);
            bool MeshExists_(const std::string& uid);       
            const Mesh& GetMeshAccessor_(const std::string& uid);
            vk::Buffer* GetVertexBuffer_();
            vk::Buffer* GetIndexBuffer_();
            static ResourceManager& Get();

            GraphicsResources m_resources;
    };

    inline ResourceManager& ResourceManager::Get()
    {
        static ResourceManager instance;
        return instance;
    }

    inline void ResourceManager::AddTextures(TexturesData textures)
    {
        Get().AddTextures_(std::move(textures));
    }

    inline bool ResourceManager::TextureExists(const std::string& uid)
    {
        return Get().TextureExists_(std::move(uid));
    }

    inline uint32_t ResourceManager::GetTextureAccessor(const std::string& uid)
    {
        return Get().GetTextureAccessor_(std::move(uid));
    }

    inline vk::DescriptorSet* ResourceManager::GetDescriptorSet()
    {
        return Get().GetDescriptorSet_();
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetDescriptorSetLayout()
    {
        return Get().GetDescriptorSetLayout_();
    }

    inline void ResourceManager::AddMeshes(MeshesData meshes)
    {
        Get().AddMeshes_(std::move(meshes));
    }

    inline bool ResourceManager::MeshExists(const std::string& uid)
    {
        return Get().MeshExists_(std::move(uid));
    }

    inline const Mesh& ResourceManager::GetMeshAccessor(const std::string& uid)
    {
        return Get().GetMeshAccessor_(std::move(uid));
    }

    inline vk::Buffer* ResourceManager::GetVertexBuffer()
    {
        return Get().GetVertexBuffer_();
    }

    inline vk::Buffer* ResourceManager::GetIndexBuffer()
    {
        return Get().GetIndexBuffer_();
    }

    inline void ResourceManager::AddTextures_(TexturesData textures)
    {
        m_resources.textures = std::move(textures);
    }

    inline bool ResourceManager::TextureExists_(const std::string& uid)
    {
        return m_resources.textures.accessors.find(std::move(uid)) != m_resources.textures.accessors.end();
    }

    inline uint32_t ResourceManager::GetTextureAccessor_(const std::string& uid)
    {
        return m_resources.textures.accessors.at(std::move(uid));
    }

    inline vk::DescriptorSet* ResourceManager::GetDescriptorSet_()
    {
        return &m_resources.textures.descriptorSet;
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetDescriptorSetLayout_()
    {
        return &m_resources.textures.descriptorSetLayout;
    }

    inline void ResourceManager::AddMeshes_(MeshesData meshes)
    {
        m_resources.meshes = std::move(meshes);
    }

    inline bool ResourceManager::MeshExists_(const std::string& uid)
    {
        return m_resources.meshes.accessors.find(std::move(uid)) != m_resources.meshes.accessors.end();
    }

    inline const Mesh& ResourceManager::GetMeshAccessor_(const std::string& uid)
    {
        return m_resources.meshes.accessors.at(std::move(uid));
    }

    inline vk::Buffer* ResourceManager::GetVertexBuffer_()
    {
        return m_resources.meshes.vertexBuffer.GetBuffer();
    }

    inline vk::Buffer* ResourceManager::GetIndexBuffer_()
    {
        return m_resources.meshes.indexBuffer.GetBuffer();
    }
}