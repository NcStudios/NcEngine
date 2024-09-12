#include "gtest/gtest.h"
#include "ncengine/config/Config.h"
#include "config/ConfigInternal.h"

#include "ncutility/NcError.h"

#include <filesystem>
#include <string>

const auto g_collateralDir = std::string{NC_TEST_COLLATERAL_DIR};

TEST(ConfigTests, Validate_defaultValues_returnsTrue)
{
    const auto actual = nc::config::Config{};
    EXPECT_TRUE(nc::config::Validate(actual));
}

TEST(ConfigTests, Validate_badValues_returnsFalse)
{
    // Just spot checking a few settings
    {
        auto actual = nc::config::Config{};
        actual.assetSettings.audioClipsPath = "";
        EXPECT_FALSE(nc::config::Validate(actual));
    }

    {
        auto actual = nc::config::Config{};
        actual.graphicsSettings.screenWidth = 0;
        EXPECT_FALSE(nc::config::Validate(actual));
    }

    {
        auto actual = nc::config::Config{};
        actual.physicsSettings.velocitySteps = 1u;
        EXPECT_FALSE(nc::config::Validate(actual));
    }
}

TEST(ConfigTests, Load_allValues_succeeds)
{
    nc::config::Load(g_collateralDir + "/config.ini");
}

TEST(ConfigTests, Load_badPath_throws)
{
    EXPECT_THROW(nc::config::Load(g_collateralDir + "/nothing/here.ini"), nc::NcError);
}

TEST(ConfigTests, Read_partialConfig_succeeds)
{
    auto stream = std::istringstream
    {
        "project_name=PartialCollateral\n"
        "log_file_path=Partial.log\n"
        "max_transforms=100\n"
        "use_validation_layers=1\n"
    };

    const auto actual = nc::config::Read(stream);
    EXPECT_STREQ("PartialCollateral", actual.projectSettings.projectName.c_str());
    EXPECT_STREQ("Partial.log", actual.projectSettings.logFilePath.c_str());
    EXPECT_EQ(100, actual.memorySettings.maxTransforms);
    EXPECT_TRUE(actual.graphicsSettings.useValidationLayers);
}

TEST(ConfigTests, Read_emptyConfig_succeeds)
{
    auto stream = std::istringstream{""};
    nc::config::Read(stream);
}

TEST(ConfigTests, Read_unknownKey_succeeds)
{
    auto stream = std::istringstream{"not_a_key=false"};
    nc::config::Read(stream);
}

TEST(ConfigTests, Read_leadingAndTrailingWhitespace_parsesCorrectValue)
{
    auto stream = std::istringstream{" \n\n  audio_clips_path=/audio/   \n "};
    const auto actual = nc::config::Read(stream);
    EXPECT_EQ(std::string{"/audio/"}, actual.assetSettings.audioClipsPath);
}

TEST(ConfigTests, Read_whitespaceInValue_preservesWhitespace)
{
    auto stream = std::istringstream{"audio_clips_path=/audio clips/"};
    const auto actual = nc::config::Read(stream);
    EXPECT_EQ(std::string{"/audio clips/"}, actual.assetSettings.audioClipsPath);
}

TEST(ConfigTests, Read_badValue_throws)
{
    auto stream = std::istringstream{"screen_width=whoops"};
    EXPECT_THROW(nc::config::Read(stream), nc::NcError);
}

TEST(ConfigTests, SaveLoad_roundTrip_preservesData)
{
    const auto filepath = (std::filesystem::temp_directory_path() / "roundtrip.ini").string();
    const auto expected = nc::config::Config{};
    nc::config::Save(filepath, expected);
    const auto actual = nc::config::Load(filepath);

    EXPECT_EQ(expected.projectSettings.projectName, actual.projectSettings.projectName);
    EXPECT_EQ(expected.projectSettings.logFilePath, actual.projectSettings.logFilePath);
    EXPECT_EQ(expected.projectSettings.logMaxFileSize, actual.projectSettings.logMaxFileSize);

    EXPECT_FLOAT_EQ(expected.engineSettings.timeStep, actual.engineSettings.timeStep);
    EXPECT_FLOAT_EQ(expected.engineSettings.maxTimeStep, actual.engineSettings.maxTimeStep);
    EXPECT_EQ(expected.engineSettings.threadCount, actual.engineSettings.threadCount);
    EXPECT_EQ(expected.engineSettings.buildTasksOnInit, actual.engineSettings.buildTasksOnInit);

    EXPECT_EQ(expected.assetSettings.audioClipsPath, actual.assetSettings.audioClipsPath);
    EXPECT_EQ(expected.assetSettings.concaveCollidersPath, actual.assetSettings.concaveCollidersPath);
    EXPECT_EQ(expected.assetSettings.hullCollidersPath, actual.assetSettings.hullCollidersPath);
    EXPECT_EQ(expected.assetSettings.meshesPath, actual.assetSettings.meshesPath);
    EXPECT_EQ(expected.assetSettings.shadersPath, actual.assetSettings.shadersPath);
    EXPECT_EQ(expected.assetSettings.skeletalAnimationsPath, actual.assetSettings.skeletalAnimationsPath);
    EXPECT_EQ(expected.assetSettings.texturesPath, actual.assetSettings.texturesPath);
    EXPECT_EQ(expected.assetSettings.cubeMapsPath, actual.assetSettings.cubeMapsPath);
    EXPECT_EQ(expected.assetSettings.fontsPath, actual.assetSettings.fontsPath);

    EXPECT_EQ(expected.memorySettings.maxRigidBodies, actual.memorySettings.maxRigidBodies);
    EXPECT_EQ(expected.memorySettings.maxNetworkDispatchers, actual.memorySettings.maxNetworkDispatchers);
    EXPECT_EQ(expected.memorySettings.maxParticleEmitters, actual.memorySettings.maxParticleEmitters);
    EXPECT_EQ(expected.memorySettings.maxRenderers, actual.memorySettings.maxRenderers);
    EXPECT_EQ(expected.memorySettings.maxTransforms, actual.memorySettings.maxTransforms);
    EXPECT_EQ(expected.memorySettings.maxPointLights, actual.memorySettings.maxPointLights);
    EXPECT_EQ(expected.memorySettings.maxSpotLights, actual.memorySettings.maxSpotLights);
    EXPECT_EQ(expected.memorySettings.maxSkeletalAnimations, actual.memorySettings.maxSkeletalAnimations);
    EXPECT_EQ(expected.memorySettings.maxTextures, actual.memorySettings.maxTextures);
    EXPECT_EQ(expected.memorySettings.maxCubeMaps, actual.memorySettings.maxCubeMaps);
    EXPECT_EQ(expected.memorySettings.maxParticles, actual.memorySettings.maxParticles);

    EXPECT_EQ(expected.graphicsSettings.enabled, actual.graphicsSettings.enabled);
    EXPECT_EQ(expected.graphicsSettings.useNativeResolution, actual.graphicsSettings.useNativeResolution);
    EXPECT_EQ(expected.graphicsSettings.launchInFullscreen, actual.graphicsSettings.launchInFullscreen);
    EXPECT_EQ(expected.graphicsSettings.screenWidth, actual.graphicsSettings.screenWidth);
    EXPECT_EQ(expected.graphicsSettings.screenHeight, actual.graphicsSettings.screenHeight);
    EXPECT_EQ(expected.graphicsSettings.targetFPS, actual.graphicsSettings.targetFPS);
    EXPECT_FLOAT_EQ(expected.graphicsSettings.nearClip, actual.graphicsSettings.nearClip);
    EXPECT_FLOAT_EQ(expected.graphicsSettings.farClip, actual.graphicsSettings.farClip);
    EXPECT_EQ(expected.graphicsSettings.useShadows, actual.graphicsSettings.useShadows);
    EXPECT_EQ(expected.graphicsSettings.antialiasing, actual.graphicsSettings.antialiasing);
    EXPECT_EQ(expected.graphicsSettings.useValidationLayers, actual.graphicsSettings.useValidationLayers);

    EXPECT_EQ(expected.physicsSettings.enabled, actual.physicsSettings.enabled);
    EXPECT_EQ(expected.physicsSettings.tempAllocatorSize, actual.physicsSettings.tempAllocatorSize);
    EXPECT_EQ(expected.physicsSettings.maxBodyPairs, actual.physicsSettings.maxBodyPairs);
    EXPECT_EQ(expected.physicsSettings.maxContacts, actual.physicsSettings.maxContacts);
    EXPECT_EQ(expected.physicsSettings.velocitySteps, actual.physicsSettings.velocitySteps);
    EXPECT_EQ(expected.physicsSettings.positionSteps, actual.physicsSettings.positionSteps);
    EXPECT_FLOAT_EQ(expected.physicsSettings.baumgarteStabilization, actual.physicsSettings.baumgarteStabilization);
    EXPECT_FLOAT_EQ(expected.physicsSettings.speculativeContactDistance, actual.physicsSettings.speculativeContactDistance);
    EXPECT_FLOAT_EQ(expected.physicsSettings.penetrationSlop, actual.physicsSettings.penetrationSlop);
    EXPECT_FLOAT_EQ(expected.physicsSettings.timeBeforeSleep, actual.physicsSettings.timeBeforeSleep);
    EXPECT_FLOAT_EQ(expected.physicsSettings.sleepThreshold, actual.physicsSettings.sleepThreshold);

    EXPECT_EQ(expected.audioSettings.enabled, actual.audioSettings.enabled);
    EXPECT_EQ(expected.audioSettings.bufferFrames, actual.audioSettings.bufferFrames);
}
