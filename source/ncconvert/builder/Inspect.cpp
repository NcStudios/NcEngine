#include "Inspect.h"
#include "utility/Log.h"
#include "utility/EnumExtensions.h"

#include "ncasset/Import.h"
#include "ncutility/NcError.h"

namespace
{
constexpr auto headerTemplate =
R"(File {}
Header
  magic number {}
  compression  {}
  version      {}
  size         {})";

constexpr auto audioClipTemplate =
R"(Data
  samples count {})";

constexpr auto concaveColliderTemplate =
R"(Data
  extents       {}, {}, {}
  max extent    {}
  vertex count  {})";

constexpr auto cubeMapTemplate =
R"(Data
  face side length {})";

constexpr auto hullColliderTemplate =
R"(Data
  extents        {}, {}, {}
  max extent     {}
  triangle count {})";

constexpr auto meshTemplate =
R"(Data
  extents                         {}, {}, {}
  max extent                      {}
  vertex count                    {}
  index count                     {}
  bones data vertex to bone count {}
  bones data bone to parent count {})";

constexpr auto skeletalAnimationTemplate =
R"(Data
  name              {}
  duration in ticks {}
  ticks per seconds {}
  frames per bone   {})";

constexpr auto textureTemplate =
R"(Data
  width  {}
  height {})";

} // anonymous namespace

namespace nc::convert
{
void Inspect(const std::filesystem::path& ncaPath)
{
    const auto header = asset::ImportNcaHeader(ncaPath);
    const auto type = GetAssetType(header);
    LOG(headerTemplate, ncaPath.string(), header.magicNumber, header.compressionAlgorithm, header.version, header.size);
    if (!asset::IsVersionSupported(header.version))
    {
        LOG("Unsupported asset version {}", header.version);
        return;
    }

    switch (type)
    {
        case asset::AssetType::AudioClip:
        {
            const auto asset = asset::ImportAudioClip(ncaPath);
            LOG(audioClipTemplate, asset.samplesPerChannel);
            break;
        }
        case asset::AssetType::ConcaveCollider:
        {
            const auto asset = asset::ImportConcaveCollider(ncaPath);
            LOG(concaveColliderTemplate, asset.extents.x, asset.extents.y, asset.extents.z, asset.maxExtent, asset.triangles.size());
            break;
        }
        case asset::AssetType::CubeMap:
        {
            const auto asset = asset::ImportCubeMap(ncaPath);
            LOG(cubeMapTemplate, asset.faceSideLength);
            break;
        }
        case asset::AssetType::HullCollider:
        {
            const auto asset = asset::ImportHullCollider(ncaPath);
            LOG(concaveColliderTemplate, asset.extents.x, asset.extents.y, asset.extents.z, asset.maxExtent, asset.vertices.size());
            break;
        }
        case asset::AssetType::Mesh:
        {
            const auto asset = asset::ImportMesh(ncaPath);
            auto vertexSpaceSize = asset.bonesData.has_value()? asset.bonesData.value().vertexSpaceToBoneSpace.size() : 0;
            auto boneSpaceSize = asset.bonesData.has_value()? asset.bonesData.value().boneSpaceToParentSpace.size() : 0;
            LOG(meshTemplate, asset.extents.x, asset.extents.y, asset.extents.z, asset.maxExtent, asset.vertices.size(), asset.indices.size(), vertexSpaceSize, boneSpaceSize);
            break;
        }
        case asset::AssetType::Shader:
        {
            LOG("Shader not supported");
            break;
        }
        case asset::AssetType::SkeletalAnimation:
        {
            const auto asset = asset::ImportSkeletalAnimation(ncaPath);
            LOG(skeletalAnimationTemplate, asset.name, asset.durationInTicks, asset.ticksPerSecond, asset.framesPerBone.size());
            break;
        }
        case asset::AssetType::Texture:
        {
            const auto asset = asset::ImportTexture(ncaPath);
            LOG(textureTemplate, asset.width, asset.height);
            break;
        }
        case asset::AssetType::Font:
        {
            LOG("Font not supported");
            break;
        }
    }
}
} // namespace nc::convert
