#include "ShaderAssetManager.h"
#include "debug/NcError.h"

#include <fstream>
#include <filesystem>

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

    auto HasValidAssetExtension(const std::string& path) -> bool
    {
        const std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension == "nca" ? true : false;
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
        else descriptorType = nc::DescriptorType::CombinedImageSampler;

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

    auto ReadShader(const std::string& path, const std::string uid) -> nc::ShaderView
    {
        const auto ncaString = (std::filesystem::path(path) / std::filesystem::path("shader.nca")).string();

        if (!HasValidAssetExtension(ncaString)) throw nc::NcError("Invalid extension: " + ncaString);

        std::ifstream file{ncaString};
        if (!file.is_open()) throw nc::NcError("Could not open file: " + ncaString);

        std::string vertexShaderPath;
        file >> vertexShaderPath;
        vertexShaderPath = (std::filesystem::path(path) / std::filesystem::path(vertexShaderPath)).string();

        std::string fragmentShaderPath;
        file >> fragmentShaderPath;
        fragmentShaderPath = (std::filesystem::path(path) / std::filesystem::path(fragmentShaderPath)).string();

        auto vertexByteCode = ReadSpirvIntoBytes(vertexShaderPath);
        auto fragmentByteCode = ReadSpirvIntoBytes(fragmentShaderPath);

        auto shaderView = nc::ShaderView
        {
            .uid ="",
            .vertexByteCode = vertexByteCode,
            .fragmentByteCode =  fragmentByteCode,
            .descriptors = {}
        };

        uint32_t descriptorCount;
        file >> descriptorCount;

        for (uint32_t i = 0; i < descriptorCount; ++i)
        {
            shaderView.descriptors.emplace_back(ReadDescriptor(file));
        }

        shaderView.uid = uid;

        return shaderView;
    }

}

namespace nc
{
    ShaderAssetManager::ShaderAssetManager(const std::string& assetDirectory)
        : m_shaderViews{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool ShaderAssetManager::Load(const std::string& path, bool isExternal)
    {
        const auto fullPath = isExternal ? path : m_assetDirectory + path;

        auto shaderView = ReadShader(fullPath, path);

        if (IsLoaded(fullPath))
        {
            return false;
        }

        for (const auto& manifest : m_shaderViews)
        {
            if (manifest.uid == shaderView.uid)
            {
                return false;
            }
        }

        if (shaderView.vertexByteCode.empty() || shaderView.fragmentByteCode.empty())
        {
            return false;
        }

        m_shaderViews.emplace_back(std::move(shaderView));

        return true;
    }

    bool ShaderAssetManager::Load(std::span<const std::string> paths, bool isExternal)
    {
        auto atLeastOneFailure = false;
        for (const auto& path : paths)
        {
            if (IsLoaded(path))
            {
                continue;
            }

            if (!Load(path, isExternal))
            {
                atLeastOneFailure = true;
            }
        }

        return !atLeastOneFailure;
    }

    bool ShaderAssetManager::Unload(const std::string& path)
    {
        if (!IsLoaded(path))
        {
            return false;
        }

        auto shaderViews = std::vector<ShaderView>();

        for (auto shaderView : shaderViews) // Intentionally copying here
        {
            if (shaderView.uid != path)
            {
                shaderViews.emplace_back(std::move(shaderView));
            }
        }

        m_shaderViews.assign(shaderViews.begin(), shaderViews.end());

        return true;
    }

    void ShaderAssetManager::UnloadAll()
    {
        m_shaderViews.clear();
    }

    auto ShaderAssetManager::Acquire(const std::string& path) const -> ShaderView
    {
        for (const auto& shaderView : m_shaderViews)
        {
            if (shaderView.uid == path)
            {
                return shaderView;
            }
        }

        throw NcError("Asset not loaded: " + path);
    }

    bool ShaderAssetManager::IsLoaded(const std::string& path) const
    {
        return std::ranges::any_of(m_shaderViews.begin(), m_shaderViews.end(), [path](const auto& view) { return view.uid == path; });
    }
}