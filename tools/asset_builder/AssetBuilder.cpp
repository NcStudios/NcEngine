#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/** The AssetBuilder is a utility to convert fbx and obj file to a simple
 *  format using AssImp. Currently, the only purpose of this is to speed
 *  up debugging by moving calls to AssImp outside of the engine. */

struct Config
{
    std::filesystem::path AssetTargetsPath;
    std::filesystem::path OutputDirectory;
};

const auto AssetExtension = std::string{".nca"};
const auto DefaultAssetTargetFilename = std::string{"targets.txt"};
constexpr auto AssimpFlags = aiProcess_Triangulate |
                             aiProcess_JoinIdenticalVertices |
                             aiProcess_ConvertToLeftHanded |
                             aiProcess_GenNormals |
                             aiProcess_CalcTangentSpace;

void Usage();
bool ParseArgs(int argc, char** argv, Config* config);
void CreateOutputDirectory(const std::filesystem::path& directory);
auto ReadTargets(const std::filesystem::path& targetsPath) -> std::vector<std::filesystem::path>;
bool IsValidMeshExtension(const std::filesystem::path& extension);
auto ToAssetPath(const std::filesystem::path& meshPath, const Config& config) -> std::filesystem::path;
void SanitizeFloat(float* value, bool* badValueDetected);
void SanitizeVector(aiVector3D* value, bool* badValueDetected);
void BuildAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config);
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
        for(const auto& targetPath : ReadTargets(config.AssetTargetsPath))
        {
            BuildAsset(&importer, targetPath, config);
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
              << "  -f <file>               Read targets from <file>\n"
              << "  -o <dir>                Write assets to <dir>\n\n"
              << "  When using -f, <file> should be a newline-separated list of paths to\n"
              << "  meshes to parse. If the option is not used, the executable's directory\n"
              << "  will searched for \"targets.txt\". In either case the mesh paths should\n"
              << "  be absolute or relative to the current directory when calling build.exe.\n";
}

bool ParseArgs(int argc, char** argv, Config* out)
{
    out->AssetTargetsPath = std::filesystem::path(argv[0]).replace_filename(DefaultAssetTargetFilename);
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

        if(option.compare("-f") == 0)
        {
            out->AssetTargetsPath = std::filesystem::path(argv[current++]);
            continue;
        }

        if(option.compare("-o") == 0)
        {
            out->OutputDirectory = std::filesystem::path(argv[current++]);
            continue;
        }

        return false;
    }

    out->AssetTargetsPath.make_preferred();
    out->OutputDirectory.make_preferred();
    return true;
}

void CreateOutputDirectory(const std::filesystem::path& directory)
{
    if(std::filesystem::exists(directory))
        return;

    std::cout << "Creating directory: " << directory << '\n';
    if(!std::filesystem::create_directories(directory))
        throw std::runtime_error("Failed to create output directory: " + directory.string());
}

auto ReadTargets(const std::filesystem::path& targetsPath) -> std::vector<std::filesystem::path>
{
    std::ifstream file{targetsPath};
    if(!file.is_open())
        throw std::runtime_error("Failure opening file: " + targetsPath.string());

    constexpr unsigned bufferSize = 512u;
    char buffer[bufferSize];
    std::vector<std::filesystem::path> out;
    std::cout << "Reading targets from: " << targetsPath.string() << '\n';

    while(!file.eof())
    {
        if(file.fail())
            throw std::runtime_error("Failure reading file: " + targetsPath.string());
        
        file.getline(buffer, bufferSize, '\n');
        auto& path = out.emplace_back(buffer);
        path.make_preferred();
        std::cout << "    " << path << '\n';
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

void BuildAsset(Assimp::Importer* importer, const std::filesystem::path& inPath, const Config& config)
{
    if (!IsValidMeshExtension(inPath.extension()))
        throw std::runtime_error("Invalid mesh file extension: " + inPath.string());

    const auto assetPath = ToAssetPath(inPath, config);
    std::cout << "Creating: " << assetPath << '\n';
    std::ofstream outFile{assetPath};
    if(!outFile)
        throw std::runtime_error("Failure opening asset file");

    const auto pModel = importer->ReadFile(inPath.string(), AssimpFlags);
    if(!pModel)
        throw std::runtime_error("AssImp failure");

    const auto pMesh = pModel->mMeshes[0];
    if(!pMesh->mNormals || !pMesh->mTextureCoords || !pMesh->mTangents || !pMesh->mBitangents)
            throw std::runtime_error("File does not contain all required data");

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

auto operator<<(std::ostream& stream, aiVector3D& vec) -> std::ostream&
{
    stream << vec.x << ' ' << vec.y << ' ' << vec.z << ' ';
    return stream;
}