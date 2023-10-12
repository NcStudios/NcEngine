#include "ShaderAssetManager.h"
#include "AssetUtilities.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <ranges>

namespace
{
    auto ReadSpirvIntoBytes(const std::string& path) -> std::vector<uint32_t>
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open() || file.tellg() == -1)
        {
            throw nc::NcError("Failed to open file: ", path);
        }

        const auto fileSize = static_cast<uint32_t>(file.tellg());
        if (fileSize % 4 != 0)
        {
            throw nc::NcError("The file of shader byte code was not uint32_t aligned: ", path);
        }

        const auto bufferSize = fileSize/sizeof(uint32_t);
        std::vector<uint32_t> buffer(bufferSize);
        const auto charBuffer = reinterpret_cast<char*>(buffer.data());
        file.seekg(0);
        file.read(charBuffer, fileSize);
        if (file.fail())
        {
            file.close();
            throw nc::NcError("The file was wonky. (failbit set)", path);
        }

        file.close();
        return buffer;
    }
    
    auto ReadDescriptor(std::ifstream& file) -> nc::DescriptorManifest
    {
        uint32_t set;
        file >> set;

        uint32_t binding;
        file >> binding;

        std::string descriptorTypeFromFile;
        file >> descriptorTypeFromFile;
        auto descriptorType = nc::DescriptorType::None;
        if (descriptorTypeFromFile == "UniformBuffer")
        {
            descriptorType = nc::DescriptorType::UniformBuffer;
        }
        else if (descriptorTypeFromFile == "StorageBuffer")
        {
            descriptorType = nc::DescriptorType::StorageBuffer;
        }
        else if (descriptorTypeFromFile == "CombinedImageSampler")
        {
            descriptorType = nc::DescriptorType::CombinedImageSampler;
        }
        if (descriptorType == nc::DescriptorType::None)
        {
            throw nc::NcError("No valid descriptor type found.");
        }

        std::string stagesFromFile;
        file >> stagesFromFile;
        auto stages = nc::ShaderStages::None;
        if (stagesFromFile.find("Vertex") != std::string::npos) 
        {
            stages = stages | nc::ShaderStages::Vertex;
        }
        if (stagesFromFile.find("Fragment") != std::string::npos) 
        {
            stages = stages | nc::ShaderStages::Fragment;
        }
        if (stages == nc::ShaderStages::None)
        {
            throw nc::NcError("No valid shader stages found.");
        }

        return nc::DescriptorManifest
        {
            .setIndex = set,
            .slotIndex = binding,
            .descriptorType = descriptorType,
            .shaderStages = stages
        };
    }

    auto ReadShader(const std::string& path, const std::string& uid) -> nc::ShaderFlyweight
    {
        const auto ncaString = (std::filesystem::path(path) / std::filesystem::path("shader.nca")).string();

        if (!nc::HasValidAssetExtension(ncaString)) throw nc::NcError("Invalid extension: " + ncaString);

        std::ifstream file{ncaString};
        if (!file.is_open()) throw nc::NcError("Could not open file: " + ncaString);

        std::string vertexShaderPath;
        file >> vertexShaderPath;
        vertexShaderPath = (std::filesystem::path(path) / std::filesystem::path(vertexShaderPath)).string();

        std::string fragmentShaderPath;
        file >> fragmentShaderPath;
        fragmentShaderPath = (std::filesystem::path(path) / std::filesystem::path(fragmentShaderPath)).string();

        auto shaderFlyweight = nc::ShaderFlyweight
        {
            .uid ="",
            .vertexByteCode = ReadSpirvIntoBytes(vertexShaderPath),
            .fragmentByteCode = ReadSpirvIntoBytes(fragmentShaderPath),
            .descriptors = {}
        };

        uint32_t descriptorCount;
        file >> descriptorCount;

        for (uint32_t i = 0; i < descriptorCount; ++i)
        {
            shaderFlyweight.descriptors.emplace_back(ReadDescriptor(file));
        }

        shaderFlyweight.uid = uid;

        return shaderFlyweight;
    }
}

namespace nc
{
    ShaderAssetManager::ShaderAssetManager(const std::string& assetDirectory)
        : m_shaderFlyweights{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool ShaderAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
    {
        const auto fullPath = isExternal ? path : m_assetDirectory + path;

        if (IsLoaded(path))
        {
            return false;
        }

        auto shaderFlyweight = ReadShader(fullPath, path);

        if (shaderFlyweight.vertexByteCode.empty() || shaderFlyweight.fragmentByteCode.empty())
        {
            return false;
        }

        m_shaderFlyweights.push_back(std::move(shaderFlyweight));

        return true;
    }

    bool ShaderAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
    {
        auto atLeastOneFailure = false;
        for (const auto& path : paths)
        {
            if (!Load(path, isExternal))
            {
                atLeastOneFailure = true;
            }
        }

        return !atLeastOneFailure;
    }

    bool ShaderAssetManager::Unload(const std::string& path, asset_flags_type)
    {
        if (!IsLoaded(path))
        {
            return false;
        }

        const auto pos = std::ranges::find_if(m_shaderFlyweights, [&path](const auto& asset)
        {
            return asset.uid == path;
        });

        if(pos == m_shaderFlyweights.end())
        {
            throw NcError("Could not find asset in manifest:", path);
        }

        m_shaderFlyweights.erase(pos);

        return true;
    }

    void ShaderAssetManager::UnloadAll(asset_flags_type)
    {
        m_shaderFlyweights.clear();
    }

    auto ShaderAssetManager::Acquire(const std::string& path, asset_flags_type) const -> ShaderView
    {
        auto pos = std::ranges::find_if(m_shaderFlyweights, [&path](const auto& asset)
        {
            return asset.uid == path;
        });

        if(pos == m_shaderFlyweights.end())
        {
            throw NcError("Asset not loaded: " + path);
        }

        const auto& flyweight = *pos;

        return ShaderView
        {
            .uid = flyweight.uid,
            .vertexByteCode = flyweight.vertexByteCode,
            .fragmentByteCode = flyweight.fragmentByteCode,
            .descriptors = flyweight.descriptors
        };
    }

    bool ShaderAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
    {
        return std::ranges::any_of(m_shaderFlyweights.begin(), m_shaderFlyweights.end(), [path](const auto& view) { return view.uid == path; });
    }

    auto ShaderAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
    {
        return GetPaths(m_shaderFlyweights, [](const auto& item)
        {
            return std::string_view{item.uid};
        });
    }
}
