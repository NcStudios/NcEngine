#include "Inspect.h"
#include "utility/Log.h"
#include "utility/EnumExtensions.h"

#include "ncasset/Import.h"
#include "ncjolt/ShapeUtility.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"

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

constexpr auto meshColliderTemplate =
R"(Data
  extents    {}, {}, {}
  max extent {}
  blob size  {})";

constexpr auto cubeMapTemplate =
R"(Data
  face side length {})";

constexpr auto convexHullTemplate =
R"(Data
  extents      {}, {}, {}
  max extent   {}
  vertex count {})";

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
  format {}
  width  {}
  height {}
  mips   {})";

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
        case asset::AssetType::ConvexHull:
        {
            const auto asset = asset::ImportConvexHull(ncaPath);
            const auto shape = jolt::DeserializeShape(asset.blob);
            if (shape->GetSubType() != JPH::EShapeSubType::ConvexHull)
            {
                LOG("Unexpected binary format for ConvexHull")
                break;
            }

            const auto hull = static_cast<const JPH::ConvexHullShape*>(shape.GetPtr());
            LOG(convexHullTemplate, asset.extents.x, asset.extents.y, asset.extents.z, asset.maxExtent, hull->GetNumPoints());
            break;
        }
        case asset::AssetType::CubeMap:
        {
            const auto asset = asset::ImportCubeMap(ncaPath);
            LOG(cubeMapTemplate, asset.faceSideLength);
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
        case asset::AssetType::MeshCollider:
        {
            const auto asset = asset::ImportMeshCollider(ncaPath);
            LOG(meshColliderTemplate, asset.extents.x, asset.extents.y, asset.extents.z, asset.maxExtent, asset.blob.size());
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
            LOG(textureTemplate, ToString(asset.format), asset.width, asset.height, 1 + asset.mipmaps.size());
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
