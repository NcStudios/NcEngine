#include "gtest/gtest.h"
#include "ReturnCodes.h"
#include "CollateralCommon.h"

#include "fmt/format.h"

#include <cstdlib>

#ifndef NC_CONVERT_EXECUTABLE_PATH
#error NC_CONVERT_EXECUTABLE_PATH must be defined for nc-convert integration tests
#endif

const auto exePath = std::filesystem::path{NC_CONVERT_EXECUTABLE_PATH};
#ifdef WIN32
const auto exeName = exePath.filename().string();
#else
const auto exeName = "./" + exePath.filename().string();
#endif
const auto ncaTestOutDirectory = collateral::collateralDirectory / "test_temp_dir";

auto RunCmd(const std::string& cmd) -> int
{
    auto result = std::system(cmd.c_str());

#ifdef WIN32
    return result;
#else
    // note: At least on Mac, this takes the address of the arg, so we need to pass a local var.
    return WEXITSTATUS(result);
#endif
}

auto BuildSingleTargetCommand(std::string_view type, std::string_view sourceName, std::string_view assetName) -> std::string
{
    const auto source = (collateral::collateralDirectory / sourceName).string();
    return fmt::format(R"({} -t {} -s "{}" -n {} -o "{}")",
        exeName, type, source, assetName, ncaTestOutDirectory.string()
    );
}

class NcConvertIntegration : public ::testing::Test
{
    public:
        NcConvertIntegration()
        {
            std::filesystem::current_path(exePath.parent_path());
            if (std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::remove_all(ncaTestOutDirectory);
            }

            std::filesystem::create_directory(ncaTestOutDirectory);
        }

        ~NcConvertIntegration()
        {
            if (std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::remove_all(ncaTestOutDirectory);
            }
        }
};

TEST_F(NcConvertIntegration, SingleTarget_audioClip_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("audio-clip", "sine_c_e.wav", "myAudioClip");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myAudioClip.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_audioClip_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("audio-clip", "cube.fbx", "myAudioClip");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_concaveCollider_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("concave-collider", "plane.fbx", "myConcaveCollider");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myConcaveCollider.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_concaveCollider_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("concave-collider", "rgb_corners_4x8.png", "myConcaveCollider");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_cubeMap_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("cube-map", "cube_map_horizontal_array.png", "myCubeMap");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myCubeMap.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_cubeMap_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("cube-map", "cube.fbx", "myCubeMap");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_hullCollider_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("hull-collider", "cube.fbx", "myHullCollider");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myHullCollider.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_hullCollider_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("hull-collider", "rgb_corners_4x8.png", "myHullCollider");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_mesh_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("mesh", "cube.fbx", "myMesh");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myMesh.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_mesh_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("mesh", "rgb_corners_4x8.png", "myMesh");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_texture_succeeds)
{
    const auto cmd = BuildSingleTargetCommand("texture", "rgb_corners_4x8.png", "myTexture");
    ASSERT_EQ(RunCmd(cmd), ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myTexture.nca"));
}

TEST_F(NcConvertIntegration, SingleTarget_texture_wrongSourceType_fails)
{
    const auto cmd = BuildSingleTargetCommand("texture", "cube.fbx", "myTexture");
    EXPECT_EQ(RunCmd(cmd), ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, SingleTarget_noType_fails)
{
    const auto source = (collateral::collateralDirectory / "cube.fbx").string();
    const auto cmd = fmt::format(R"({} -s "{}" -n {})", exeName, source, "myMesh");
    EXPECT_EQ(RunCmd(cmd), ResultCode::ArgumentError);
}

TEST_F(NcConvertIntegration, SingleTarget_noSource_fails)
{
    const auto cmd = fmt::format(R"({} -t {} -n {})", exeName, "mesh", "myMesh");
    EXPECT_EQ(RunCmd(cmd), ResultCode::ArgumentError);
}

TEST_F(NcConvertIntegration, SingleTarget_noName_fails)
{
    const auto source = (collateral::collateralDirectory / "cube.fbx").string();
    const auto cmd = fmt::format(R"({} -t mesh -s "{}")", exeName, "mesh", source);
    EXPECT_EQ(RunCmd(cmd), ResultCode::ArgumentError);
}

TEST_F(NcConvertIntegration, Manifest_succeeds)
{
    const auto manifestPath = (collateral::collateralDirectory / "manifest.json").string();
    const auto cmd = fmt::format(R"({} -m "{}")", exeName, manifestPath);
    const auto result = RunCmd(cmd);
    ASSERT_EQ(result, ResultCode::Success);
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myAudioClip.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myConcaveCollider.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myCubeMap.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myHullCollider.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myMesh.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myMultiOutputMesh.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "myTexture.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "cube1.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "cube1a.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "cube2.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "cube3.nca"));
    EXPECT_TRUE(std::filesystem::exists(ncaTestOutDirectory / "wiggle.nca"));
}

TEST_F(NcConvertIntegration, Manifest_subResourceMeshNotPresent_manifestFails)
{
    // Added a mesh entry called "idontexist" in the manifest.

    const auto manifestPath = (collateral::collateralDirectory / "manifest_mesh_not_present.json").string();
    const auto cmd = fmt::format(R"({} -m "{}")", exeName, manifestPath);
    const auto result = RunCmd(cmd);
    ASSERT_EQ(result, ResultCode::RuntimeError);
}

TEST_F(NcConvertIntegration, Manifest_noManifestPath_fails)
{
    const auto cmd = fmt::format("{} -m", exeName);
    const auto result = RunCmd(cmd);
    EXPECT_EQ(result, ResultCode::ArgumentError);
}

TEST_F(NcConvertIntegration, Inspect_succeeds)
{
    const auto buildCmd = BuildSingleTargetCommand("texture", "rgb_corners_4x8.png", "myTexture");
    const auto buildResult = RunCmd(buildCmd);
    ASSERT_EQ(buildResult, ResultCode::Success);

    const auto targetPath = (ncaTestOutDirectory / "myTexture.nca").string();
    const auto inspectCmd = fmt::format(R"({} -i "{}")", exeName, targetPath);
    const auto inspectResult = RunCmd(inspectCmd);
    EXPECT_EQ(inspectResult, ResultCode::Success);
}

TEST_F(NcConvertIntegration, Inspect_noTarget_fails)
{
    const auto cmd = fmt::format("{} -i", exeName);
    EXPECT_EQ(RunCmd(cmd), ResultCode::ArgumentError);
}
