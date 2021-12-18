#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum class AssetType
{
    Mesh,
    ConvexHull,
    ConcaveCollider,
    Skybox
};

struct Target
{
    std::filesystem::path path;
    AssetType type;
};

struct Config
{
    std::optional<std::filesystem::path> SingleTargetPath;
    std::optional<AssetType> SingleTargetType;
    std::optional<std::filesystem::path> TargetsFilePath;
    std::filesystem::path OutputDirectory;
};

struct MeshExtents
{
    float xExtent = 0.0f;
    float yExtent = 0.0f;
    float zExtent = 0.0f;
    float maxExtent = 0.0f;
};

const auto AssetExtension = std::string{".nca"};
const auto DefaultAssetTargetFilename = std::string{"targets.txt"};
constexpr auto MeshFlags = aiProcess_Triangulate |
                           aiProcess_JoinIdenticalVertices |
                           aiProcess_ConvertToLeftHanded |
                           aiProcess_GenNormals |
                           aiProcess_CalcTangentSpace;

constexpr auto ConvexHullFlags = aiProcess_Triangulate |
                                 aiProcess_JoinIdenticalVertices |
                                 aiProcess_ConvertToLeftHanded;

constexpr auto ConcaveColliderFlags = aiProcess_Triangulate |
                                      aiProcess_ConvertToLeftHanded;

void Usage();
bool ParseArgs(int argc, char** argv, Config* config);
auto GetAssetType(std::string type) -> AssetType;
void CreateOutputDirectory(const std::filesystem::path& directory);
auto ReadTargets(const Config& config) -> std::vector<Target>;
bool IsValidMeshExtension(const std::filesystem::path& extension);
auto ToAssetPath(const std::filesystem::path& meshPath, const Config& config) -> std::filesystem::path;
void SanitizeFloat(float* value, bool* badValueDetected);
void SanitizeVector(aiVector3D* value, bool* badValueDetected);
void BuildAsset(Assimp::Importer* importer, const Target& inPath, const Config& config);
void BuildMeshAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config);
void BuildConvexHullAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config);
void BuildConcaveColliderAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config);
void BuildSkyboxAsset(const std::filesystem::path& inPath, const Config& config);
auto GetMaximumVertexInDirection(const aiVector3D* data, unsigned count, aiVector3D direction) -> aiVector3D;
auto GetMeshVertexExtents(const aiVector3D* data, unsigned count) -> MeshExtents;
auto operator<<(std::ostream& stream, aiVector3D& vec) -> std::ostream&;

int main(int argc, char** argv)
{
    Config config;
    if(!ParseArgs(argc, argv, &config))
    {
        Usage();
        return 0;
    }

    try
    {
        CreateOutputDirectory(config.OutputDirectory);
        Assimp::Importer importer;
        for(const auto& target : ReadTargets(config))
        {
            BuildAsset(&importer, target, config);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void Usage()
{
    std::cout << "Usage: build.exe [options]\n"
              << "Options:\n"
              << "  -h or --help            Display this information\n"
              << "  -t <target>             Parse a single asset from <target>\n"
              << "  -a <asset type>         Specify asset type for a single target.\n"
              << "  -m <manifest>           Parse multiple assets from <manifest>\n"
              << "  -o <dir>                Output assets to <dir>\n\n"
              
              << "  Valid asset types are 'mesh', 'hull-collider', 'concave-collider', and 'skybox' and are\n"
              << "  case-insensitive.\n\n"

              << "  When using -m, <manifest> should be the path to a newline-separated list of\n"
              << "  pairs in the form '<asset-type> <path-to-input-file>'.\n\n"
              
              << "  If neither -t nor -m are used, the executable's directory will be searched\n"
              << "  for a manifest named \"targets.txt\".\n\n"
              
              << "  All paths should be absolute or relative to the current directory when\n"
              << "  calling build.exe.\n";
}

bool ParseArgs(int argc, char** argv, Config* out)
{
    out->OutputDirectory = std::filesystem::path("./");

    if(argc == 1)
        return true;
    
    std::string option;
    int current = 1;
    while (current < argc)
    {
        option = argv[current];

        if(option.compare("-h") == 0 || option.compare("--help") == 0)
            return false;

        if(++current >= argc)
            return false;

        if(option.compare("-t") == 0)
        {
            out->SingleTargetPath = std::filesystem::path(argv[current++]);
            out->SingleTargetPath.value().make_preferred();
            continue;
        }

        if(option.compare("-a") == 0)
        {
            std::cout << "-a used" << std::endl;
            out->SingleTargetType = GetAssetType(std::string{argv[current++]});
            continue;
        }

        if(option.compare("-m") == 0)
        {
            out->TargetsFilePath = std::filesystem::path(argv[current++]);
            out->TargetsFilePath.value().make_preferred();
            continue;
        }

        if(option.compare("-o") == 0)
        {
            out->OutputDirectory = std::filesystem::path(argv[current++]);
            out->OutputDirectory.make_preferred();
            continue;
        }

        return false;
    }

    if(!out->SingleTargetPath && !out->TargetsFilePath)
    {
        std::cout << " SingleTargetsPath: " << out->SingleTargetPath.value().string() << ", TargetsFilePath: " << out->SingleTargetPath.value().string() << std::endl;   
        out->TargetsFilePath = std::filesystem::path(argv[0]).replace_filename(DefaultAssetTargetFilename);
        out->TargetsFilePath.value().make_preferred();
        std::cout << " SingleTargetsPath: " << out->SingleTargetPath.value().string() << ", TargetsFilePath: " << out->SingleTargetPath.value().string() << std::endl;   
    }

    return true;
}

auto GetAssetType(std::string type) -> AssetType
{
    std::ranges::transform(type, type.begin(), [](char c) { return std::tolower(c); });

    std::cout << "GetAssetType()" << type << std::endl;

    if(type.compare("mesh") == 0)
        return AssetType::Mesh;
    else if(type.compare("hull-collider") == 0)
        return AssetType::ConvexHull;
    else if(type.compare("concave-collider") == 0)
        return AssetType::ConcaveCollider;
    else if(type.compare("skybox") == 0)
        return AssetType::Skybox;
    
    throw std::runtime_error("Failed to parse asset type: " + type);
}

void CreateOutputDirectory(const std::filesystem::path& directory)
{
    if(std::filesystem::exists(directory))
        return;

    std::cout << "Creating directory: " << directory << '\n';
    if(!std::filesystem::create_directories(directory))
        throw std::runtime_error("Failed to create output directory: " + directory.string());
}

auto ReadTargets(const Config& config) -> std::vector<Target>
{
    std::vector<Target> out;

    if(config.SingleTargetPath)
    {
        if(!config.SingleTargetType)
            throw std::runtime_error("Single target must specify asset type with -c");

        out.emplace_back(config.SingleTargetPath.value(), config.SingleTargetType.value());
    }

    if(!config.TargetsFilePath)
        return out;

    const auto& filePath = config.TargetsFilePath.value();
    std::ifstream file{filePath};
    if(!file.is_open())
        throw std::runtime_error("Failure opening file: " + filePath.string());

    constexpr unsigned bufferSize = 512u;
    char buffer[bufferSize];
    std::cout << "Reading targets from: " << filePath.string() << '\n';

    while(!file.eof())
    {
        if(file.fail())
            throw std::runtime_error("Failure reading file: " + filePath.string());
        
        Target newTarget;
        file.getline(buffer, bufferSize, ' ');
        newTarget.type = GetAssetType(std::string{buffer});

        file.getline(buffer, bufferSize, '\n');
        newTarget.path = buffer;
        newTarget.path.make_preferred();
        std::cout << "    " << newTarget.path << '\n';
        out.push_back(newTarget);
    }

    file.close();
    return out;
}

bool IsValidMeshExtension(const std::filesystem::path& extension) 
{
    return extension.compare(".fbx") == 0 ||
           extension.compare(".FBX") == 0 ||
           extension.compare(".obj") == 0 ||
           extension.compare(".OBJ") == 0;
}

auto ToAssetPath(const std::filesystem::path& meshPath, const Config& config) -> std::filesystem::path
{
    std::filesystem::path outPath{config.OutputDirectory};
    outPath /= meshPath.filename();
    outPath.replace_extension(AssetExtension);
    return outPath;
}

void SanitizeFloat(float* value, bool* badValueDetected)
{
    if(std::isnan(*value))
    {
        *value = 0.0f;
        *badValueDetected = true;
    }
}

void SanitizeVector(aiVector3D* value, bool* badValueDetected)
{
    SanitizeFloat(&value->x, badValueDetected);
    SanitizeFloat(&value->y, badValueDetected);
    SanitizeFloat(&value->z, badValueDetected);
}

void BuildAsset(Assimp::Importer* importer, const Target& target, const Config& config)
{
    switch(target.type)
    {
        case AssetType::Mesh:
        {
            BuildMeshAsset(importer, target.path, config);
            break;
        }
        case AssetType::ConvexHull:
        {
            BuildConvexHullAsset(importer, target.path, config);
            break;
        }
        case AssetType::ConcaveCollider:
        {
            BuildConcaveColliderAsset(importer, target.path, config);
            break;
        }
        case AssetType::Skybox:
        {
            BuildSkyboxAsset(target.path, config);
            break;
        }
    }
}

void BuildMeshAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config)
{
    if (!IsValidMeshExtension(inPath.extension()))
        throw std::runtime_error("Invalid mesh file extension: " + inPath.string());

    const auto assetPath = ToAssetPath(inPath, config);
    std::cout << "Creating Mesh: " << assetPath << '\n';
    std::ofstream outFile{assetPath};
    if(!outFile)
        throw std::runtime_error("Failure opening asset file");

    const auto pModel = importer->ReadFile(inPath.string(), MeshFlags);
    if(!pModel)
        throw std::runtime_error("AssImp failure");

    const auto pMesh = pModel->mMeshes[0];
    if(!pMesh->mNormals || !pMesh->mTextureCoords || !pMesh->mTangents || !pMesh->mBitangents)
            throw std::runtime_error("File does not contain all required data");

    auto extents = GetMeshVertexExtents(pMesh->mVertices, pMesh->mNumVertices);
    outFile << extents.maxExtent << '\n';
    
    bool valueWasSanitized = false;
    outFile << pMesh->mNumVertices << '\n';
    for (size_t i = 0u; i < pMesh->mNumVertices; ++i)
    {
        auto& ver = pMesh->mVertices[i];
        auto& nrm = pMesh->mNormals[i];
        auto& tex = pMesh->mTextureCoords[0][i];
        auto& tan = pMesh->mTangents[i];
        auto& bit = pMesh->mBitangents[i];
        SanitizeVector(&ver, &valueWasSanitized);
        SanitizeVector(&nrm, &valueWasSanitized);
        SanitizeVector(&tex, &valueWasSanitized);
        SanitizeVector(&tan, &valueWasSanitized);
        SanitizeVector(&bit, &valueWasSanitized);
        outFile << ver << nrm << tex.x << ' ' << tex.y << ' ' << tan << bit << '\n';
    }

    // Mult by 3 because we're triangulating
    outFile << pMesh->mNumFaces * 3 << '\n';
    for (size_t i = 0u; i < pMesh->mNumFaces; ++i)
    {
        const auto& face = pMesh->mFaces[i];
        if(face.mNumIndices != 3)
            throw std::runtime_error("Failure parsing indices");

        outFile << face.mIndices[0] << ' ' << face.mIndices[1] << ' ' << face.mIndices[2] << '\n';
    }

    if(valueWasSanitized)
        std::cerr << "    Warning: Bad value detected in mesh data. Some values have been set to 0.\n";

    outFile.close();
}

void BuildConvexHullAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config)
{
    if (!IsValidMeshExtension(inPath.extension()))
        throw std::runtime_error("Invalid hull file extension: " + inPath.string());

    const auto assetPath = ToAssetPath(inPath, config);
    std::cout << "Creating Hull Collider: " << assetPath << '\n';
    std::ofstream outFile{assetPath};
    if(!outFile)
        throw std::runtime_error("Failure opening asset file");

    const auto pModel = importer->ReadFile(inPath.string(), ConvexHullFlags);
    if(!pModel)
        throw std::runtime_error("AssImp failure");

    const auto pMesh = pModel->mMeshes[0];
    auto extents = GetMeshVertexExtents(pMesh->mVertices, pMesh->mNumVertices);

    outFile << extents.xExtent << ' ' << extents.yExtent << ' ' << extents.zExtent << '\n'
            << extents.maxExtent << '\n'
            << pMesh->mNumVertices << '\n';

    bool valueWasSanitized = false;
    for (size_t i = 0u; i < pMesh->mNumVertices; ++i)
    {
        auto& ver = pMesh->mVertices[i];
        SanitizeVector(&ver, &valueWasSanitized);
        outFile << ver << '\n';
    }

    if(valueWasSanitized)
        std::cerr << "    Warning: Bad value detected in mesh data. Some values have been set to 0.\n";

    outFile.close();
}

void BuildConcaveColliderAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config)
{
    if (!IsValidMeshExtension(inPath.extension()))
        throw std::runtime_error("Invalid mesh file extension: " + inPath.string());

    const auto assetPath = ToAssetPath(inPath, config);
    std::cout << "Creating Concave Collider: " << assetPath << '\n';
    std::ofstream outFile{assetPath};
    if(!outFile)
        throw std::runtime_error("Failure opening asset file");

    const auto pModel = importer->ReadFile(inPath.string(), ConcaveColliderFlags);
    if(!pModel)
        throw std::runtime_error("AssImp failure");

    const auto pMesh = pModel->mMeshes[0];
    auto* vertices = pMesh->mVertices;
    auto extents = GetMeshVertexExtents(vertices, pMesh->mNumVertices);
    outFile << pMesh->mNumFaces << '\n'
            << extents.maxExtent << '\n';

    bool valueWasSanitized = false;

    for (size_t i = 0u; i < pMesh->mNumFaces; ++i)
    {
        const auto& face = pMesh->mFaces[i];
        if(face.mNumIndices != 3)
            throw std::runtime_error("Failure parsing indices");

        auto& a = vertices[face.mIndices[0]];
        auto& b = vertices[face.mIndices[1]];
        auto& c = vertices[face.mIndices[2]];
        SanitizeVector(&a, &valueWasSanitized);
        SanitizeVector(&b, &valueWasSanitized);
        SanitizeVector(&c, &valueWasSanitized);

        outFile << a << ' ' << b << ' ' << c << '\n';
    }

    if(valueWasSanitized)
        std::cerr << "    Warning: Bad value detected in mesh data. Some values have been set to 0.\n";

    outFile.close();
}

void BuildSkyboxAsset(const std::filesystem::path& inPath, const Config& config)
{
    if (!std::filesystem::is_directory(inPath))
    {
        throw std::runtime_error("Must specify a directory.");
    }

    const auto assetPath = ToAssetPath(inPath, config);
    std::cout << "Creating skybox asset: " << assetPath << '\n';
    std::ofstream outFile{assetPath};
    if(!outFile)
        throw std::runtime_error("Failure opening asset file");

    struct Entry
    {
        std::string face;
        bool found = false;
    };

    std::array<Entry, 6> skyboxFacePaths
    {
        Entry{"front"}, 
        Entry{"back"}, 
        Entry{"up"}, 
        Entry{"down"}, 
        Entry{"right"}, 
        Entry{"left"}
    };

    for (const auto& facePathIt : std::filesystem::directory_iterator{inPath})
    {
        const auto& facePath = facePathIt.path();
        auto facePathName = facePath.stem().string();
        auto facePathExt = facePath.extension().string();
        std::ranges::for_each(facePathName, [](auto& character){character = std::tolower(character);});

        auto pos = std::ranges::find_if(skyboxFacePaths, [facePathName](const auto& entry)
        {
            return entry.face == facePathName;
        });

        if (pos == skyboxFacePaths.end())
        {
            continue;
        }

        pos->found = true;
        auto outputDir = config.OutputDirectory / facePathName;
        outputDir.replace_extension(facePathExt);
        auto outputDirStr = outputDir.string();
        try
        {
            std::filesystem::copy_file(facePath, outputDir, std::filesystem::copy_options::overwrite_existing);
        }
        catch(const std::exception&)
        {
        }
        outputDirStr.erase(std::remove(outputDirStr.begin(), outputDirStr.end(), '\"'), outputDirStr.end());
        outFile << facePathName << facePathExt << '\n';
    }

    if (std::any_of(skyboxFacePaths.begin(), skyboxFacePaths.end(), [](const auto& entry) { return entry.found == false; })) throw std::runtime_error("One or more skybox faces did not get created correctly.");
}

auto GetMaximumVertexInDirection(const aiVector3D* data, unsigned count, aiVector3D direction) -> aiVector3D
{
    if(count == 0u)
        throw std::runtime_error("Vertex count in hull collider is zero");
    
    unsigned maxIndex = 0u;
    float maxDot = data[0] * direction;

    for(unsigned i = 1u; i < count; ++i)
    {
        float dot = data[i] * direction;
        if(dot > maxDot)
        {
            maxDot = dot;
            maxIndex = i;
        }
    }

    return data[maxIndex];
}

auto GetMeshVertexExtents(const aiVector3D* data, unsigned count) -> MeshExtents
{
    auto minX = GetMaximumVertexInDirection(data, count, aiVector3D{ -1,  0,  0});
    auto maxX = GetMaximumVertexInDirection(data, count, aiVector3D{  1,  0,  0});
    auto minY = GetMaximumVertexInDirection(data, count, aiVector3D{  0, -1,  0});
    auto maxY = GetMaximumVertexInDirection(data, count, aiVector3D{  0,  1,  0});
    auto minZ = GetMaximumVertexInDirection(data, count, aiVector3D{  0,  0, -1});
    auto maxZ = GetMaximumVertexInDirection(data, count, aiVector3D{  0,  0,  1});

    if(minX.x > 0.0f || maxX.x < 0.0f || minY.y > 0.0f || maxY.y < 0.0f || minZ.z > 0.0f || maxZ.z < 0.0f)
        std::cerr << "Warning: Hull collider mesh does not contain the origin. The origin is assumed to be the center of mass.\n";

    std::array<float, 6u> squareMagnitudes
    {
        minX.SquareLength(), maxX.SquareLength(), minY.SquareLength(), maxY.SquareLength(), minZ.SquareLength(), maxZ.SquareLength()
    };

    float maxSquareMagnitude = squareMagnitudes[0];
    for(unsigned i = 1u; i < 6u; ++i)
    {
        if(squareMagnitudes[i] > maxSquareMagnitude)
            maxSquareMagnitude = squareMagnitudes[i];
    }

    return MeshExtents
    {
        .xExtent = maxX.x - minX.x,
        .yExtent = maxY.y - minY.y,
        .zExtent = maxZ.z - minZ.z,
        .maxExtent = sqrtf(maxSquareMagnitude)
    };
}

auto operator<<(std::ostream& stream, aiVector3D& vec) -> std::ostream&
{
    stream << vec.x << ' ' << vec.y << ' ' << vec.z << ' ';
    return stream;
}