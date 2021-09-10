#pragma once
#include "graphics/resources/GraphicsResources.h"

namespace nc::graphics
{
    class ResourceManager
    {
        public:
            ResourceManager()
            {
                m_resourceManager = this;
            }

            static std::vector<std::string> GetTexturePaths(); 
            static bool HasTextures();
            static void LoadTextures(std::unique_ptr<TexturesData> textures);
            static bool TextureExists(const std::string& uid);
            static uint32_t GetTextureAccessor(const std::string& uid);
            static vk::DescriptorSet* GetTexturesDescriptorSet();
            static vk::DescriptorSetLayout* GetTexturesDescriptorSetLayout();

            static std::vector<std::string> GetMeshPaths(); 
            static bool HasMeshes();
            static void LoadMeshes(std::unique_ptr<MeshesData> meshes);
            static bool MeshExists(const std::string& uid);
            static const Mesh& GetMeshAccessor(const std::string& uid);
            static vk::Buffer* GetVertexBuffer();
            static vk::Buffer* GetIndexBuffer();

            static void InitializePointLights(graphics::Graphics* graphics, uint32_t maxPointLights);
            static void UpdatePointLights(const std::vector<nc::PointLightInfo>& pointLights);
            static vk::DescriptorSet* GetPointLightsDescriptorSet();
            static vk::DescriptorSetLayout* GetPointLightsDescriptorSetLayout();
            static void ResetPointLights(graphics::Graphics* graphics, uint32_t maxPointLights);

            static void InitializeObjects(graphics::Graphics* graphics);
            static void UpdateObjects(const std::vector<ObjectData>& objects);
            static vk::DescriptorSet* GetObjectsDescriptorSet();
            static vk::DescriptorSetLayout* GetObjectsDescriptorSetLayout();
            static void ResetObjects(graphics::Graphics* graphics);

            static void InitializeShadowMap(graphics::Graphics* graphics);
            static void ResizeShadowMap(Vector2 dimensions);
            static vk::DescriptorSetLayout* GetShadowMapDescriptorSetLayout();
            static vk::DescriptorSet* GetShadowMapDescriptorSet();
            static const vk::ImageView& GetShadowMapImageView();

            static void Clear();

        private:
            std::vector<std::string> GetTexturePaths_(); 
            bool HasTextures_();
            void LoadTextures_(std::unique_ptr<TexturesData> textures);
            bool TextureExists_(const std::string& uid);
            uint32_t GetTextureAccessor_(const std::string& uid);
            vk::DescriptorSet* GetTexturesDescriptorSet_();
            vk::DescriptorSetLayout* GetTexturesDescriptorSetLayout_();

            std::vector<std::string> GetMeshPaths_(); 
            bool HasMeshes_();
            void LoadMeshes_(std::unique_ptr<MeshesData> meshes);
            bool MeshExists_(const std::string& uid);       
            const Mesh& GetMeshAccessor_(const std::string& uid);
            vk::Buffer* GetVertexBuffer_();
            vk::Buffer* GetIndexBuffer_();

            void InitializePointLights_(graphics::Graphics* graphics, uint32_t maxPointLights);
            void UpdatePointLights_(const std::vector<nc::PointLightInfo>& pointLights);
            vk::DescriptorSet* GetPointLightsDescriptorSet_();
            vk::DescriptorSetLayout* GetPointLightsDescriptorSetLayout_();
            void ResetPointLights_(graphics::Graphics* graphics, uint32_t maxPointLights);

            void InitializeObjects_(graphics::Graphics* graphics);
            void UpdateObjects_(const std::vector<ObjectData>& objects);
            vk::DescriptorSet* GetObjectsDescriptorSet_();
            vk::DescriptorSetLayout* GetObjectsDescriptorSetLayout_();
            void ResetObjects_(graphics::Graphics* graphics);

            void InitializeShadowMap_(graphics::Graphics* graphics);
            void ResizeShadowMap_(Vector2 dimensions);
            vk::DescriptorSetLayout* GetShadowMapDescriptorSetLayout_();
            vk::DescriptorSet* GetShadowMapDescriptorSet_();
            const vk::ImageView& GetShadowMapImageView_();

            void Clear_();

            static ResourceManager& Get();

            inline static ResourceManager* m_resourceManager = nullptr;
            std::unique_ptr<MeshesData> m_meshResources;
            std::unique_ptr<TexturesData> m_textureResources;
            std::unique_ptr<PointLightsData> m_pointLightResources;
            std::unique_ptr<ObjectsData> m_objects;
            std::unique_ptr<ShadowMapData> m_shadowMapData;
    };

    inline ResourceManager& ResourceManager::Get()
    {
        if (m_resourceManager == nullptr)
        {
            throw std::runtime_error("ResourceManager::Get: m_resourceManager was null.");
        }
        return *m_resourceManager;
    }

    inline std::vector<std::string> ResourceManager::GetTexturePaths()
    {
        return Get().GetTexturePaths_();
    }

    inline bool ResourceManager::HasTextures()
    {
        return Get().HasTextures_();
    }

    inline void ResourceManager::LoadTextures(std::unique_ptr<TexturesData> textures)
    {
        Get().LoadTextures_(std::move(textures));
    }

    inline bool ResourceManager::TextureExists(const std::string& uid)
    {
        return Get().TextureExists_(uid);
    }

    inline uint32_t ResourceManager::GetTextureAccessor(const std::string& uid)
    {
        return Get().GetTextureAccessor_(uid);
    }

    inline vk::DescriptorSet* ResourceManager::GetTexturesDescriptorSet()
    {
        return Get().GetTexturesDescriptorSet_();
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetTexturesDescriptorSetLayout()
    {
        return Get().GetTexturesDescriptorSetLayout_();
    }

    inline std::vector<std::string> ResourceManager::GetMeshPaths()
    {
        return Get().GetMeshPaths_();
    }

    inline bool ResourceManager::HasMeshes()
    {
        return Get().HasMeshes_();
    }

    inline void ResourceManager::LoadMeshes(std::unique_ptr<MeshesData> meshes)
    {
        Get().LoadMeshes_(std::move(meshes));
    }

    inline bool ResourceManager::MeshExists(const std::string& uid)
    {
        return Get().MeshExists_(uid);
    }

    inline const Mesh& ResourceManager::GetMeshAccessor(const std::string& uid)
    {
        return Get().GetMeshAccessor_(uid);
    }

    inline vk::Buffer* ResourceManager::GetVertexBuffer()
    {
        return Get().GetVertexBuffer_();
    }

    inline vk::Buffer* ResourceManager::GetIndexBuffer()
    {
        return Get().GetIndexBuffer_();
    }

    inline void ResourceManager::InitializePointLights(graphics::Graphics* graphics, uint32_t maxPointLights)
    {
        Get().InitializePointLights_(graphics, maxPointLights);
    }

    inline void ResourceManager::UpdatePointLights(const std::vector<nc::PointLightInfo>& pointLights)
    {
        Get().UpdatePointLights_(pointLights);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetPointLightsDescriptorSetLayout()
    {
        return Get().GetPointLightsDescriptorSetLayout_();
    }

    inline vk::DescriptorSet* ResourceManager::GetPointLightsDescriptorSet()
    {
        return Get().GetPointLightsDescriptorSet_();
    }

    inline void ResourceManager::ResetPointLights(graphics::Graphics* graphics, uint32_t maxPointLights)
    {
        return Get().ResetPointLights_(graphics, maxPointLights);
    }

    inline void ResourceManager::InitializeObjects(graphics::Graphics* graphics)
    {
        Get().InitializeObjects_(graphics);
    }

    inline void ResourceManager::UpdateObjects(const std::vector<ObjectData>& objectsData)
    {
        Get().UpdateObjects_(objectsData);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetObjectsDescriptorSetLayout()
    {
        return Get().GetObjectsDescriptorSetLayout_();
    }

    inline vk::DescriptorSet* ResourceManager::GetObjectsDescriptorSet()
    {
        return Get().GetObjectsDescriptorSet_();
    }

    inline void ResourceManager::ResetObjects(graphics::Graphics* graphics)
    {
        return Get().ResetObjects_(graphics);
    }

    inline void ResourceManager::Clear()
    {
        return Get().Clear_();
    }

    inline void ResourceManager::InitializeShadowMap(graphics::Graphics* graphics)
    {
        return Get().InitializeShadowMap_(graphics);
    }

    inline void ResourceManager::ResizeShadowMap(Vector2 dimensions)
    {
        return Get().ResizeShadowMap_(dimensions);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetShadowMapDescriptorSetLayout()
    {
        return Get().GetShadowMapDescriptorSetLayout_();
    }

    inline vk::DescriptorSet* ResourceManager::GetShadowMapDescriptorSet()
    {
        return Get().GetShadowMapDescriptorSet_();
    }

    inline const vk::ImageView& ResourceManager::GetShadowMapImageView()
    {
        return Get().GetShadowMapImageView_();
    }

    inline std::vector<std::string> ResourceManager::GetTexturePaths_()
    {
        return Get().GetTexturePaths_();
    }

    inline bool ResourceManager::HasTextures_()
    {
        return m_textureResources != nullptr;
    }

    inline void ResourceManager::LoadTextures_(std::unique_ptr<TexturesData> textures)
    {
        m_textureResources = std::move(textures);
    }

    inline bool ResourceManager::TextureExists_(const std::string& uid)
    {
        if (!m_textureResources) return false;
        return m_textureResources->TextureExists(uid);
    }

    inline uint32_t ResourceManager::GetTextureAccessor_(const std::string& uid)
    {
        return m_textureResources->GetAccessor(uid);
    }

    inline vk::DescriptorSet* ResourceManager::GetTexturesDescriptorSet_()
    {
        return m_textureResources->GetDescriptorSet();
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetTexturesDescriptorSetLayout_()
    {
        return m_textureResources->GetDescriptorLayout();
    }

    inline std::vector<std::string> ResourceManager::GetMeshPaths_()
    {
        return m_meshResources->GetMeshPaths();
    }

    inline bool ResourceManager::HasMeshes_()
    {
        return m_meshResources != nullptr;
    }

    inline void ResourceManager::LoadMeshes_(std::unique_ptr<MeshesData> meshes)
    {
        m_meshResources = std::move(meshes);
    }

    inline bool ResourceManager::MeshExists_(const std::string& uid)
    {
        if (!m_meshResources) return false;
        return m_meshResources->MeshExists(uid);
    }

    inline const Mesh& ResourceManager::GetMeshAccessor_(const std::string& uid)
    {
        return m_meshResources->GetAccessor(uid);
    }

    inline vk::Buffer* ResourceManager::GetVertexBuffer_()
    {
        return m_meshResources->GetVertexBuffer();
    }

    inline vk::Buffer* ResourceManager::GetIndexBuffer_()
    {
        return m_meshResources->GetIndexBuffer();
    }

    inline void ResourceManager::InitializePointLights_(graphics::Graphics* graphics, uint32_t maxPointLights)
    {
        m_pointLightResources = std::make_unique<PointLightsData>(graphics, maxPointLights);
    }

    inline void ResourceManager::UpdatePointLights_(const std::vector<nc::PointLightInfo>& pointLights)
    {
        m_pointLightResources->Update(pointLights);
    }

    inline void ResourceManager::ResetPointLights_(graphics::Graphics* graphics, uint32_t maxPointLights)
    {
        m_pointLightResources.reset();
        m_pointLightResources = std::make_unique<PointLightsData>(graphics, maxPointLights);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetPointLightsDescriptorSetLayout_()
    {
        return m_pointLightResources->GetDescriptorLayout();
    }

    inline vk::DescriptorSet* ResourceManager::GetPointLightsDescriptorSet_()
    {
        return m_pointLightResources->GetDescriptorSet();
    }

    inline void ResourceManager::InitializeObjects_(graphics::Graphics* graphics)
    {
        m_objects = std::make_unique<ObjectsData>(graphics);
    }

    inline void ResourceManager::UpdateObjects_(const std::vector<ObjectData>& objects)
    {
        m_objects->Update(objects);
    }

    inline void ResourceManager::ResetObjects_(graphics::Graphics* graphics)
    {
        m_objects.reset();
        m_objects = std::make_unique<ObjectsData>(graphics);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetObjectsDescriptorSetLayout_()
    {
        return m_objects->GetDescriptorLayout();
    }

    inline vk::DescriptorSet* ResourceManager::GetObjectsDescriptorSet_()
    {
        return m_objects->GetDescriptorSet();
    }

    inline void ResourceManager::Clear_()
    {
        m_pointLightResources.reset();
        m_objects.reset();
    }

    inline void ResourceManager::InitializeShadowMap_(graphics::Graphics* graphics)
    {
        m_shadowMapData = std::make_unique<ShadowMapData>(graphics);
    }

    inline void ResourceManager::ResizeShadowMap_(Vector2 dimensions)
    {
        return m_shadowMapData->ResizeShadowMap(dimensions);
    }

    inline vk::DescriptorSetLayout* ResourceManager::GetShadowMapDescriptorSetLayout_()
    {
        return m_shadowMapData->GetDescriptorLayout();
    }

    inline vk::DescriptorSet* ResourceManager::GetShadowMapDescriptorSet_()
    {
        return m_shadowMapData->GetDescriptorSet();
    }

    inline const vk::ImageView& ResourceManager::GetShadowMapImageView_()
    {
        return m_shadowMapData->GetShadowMapImageView();
    }
}