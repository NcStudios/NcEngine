#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationStorage.h"
#include "ncutility/Hash.h"

#include <chrono>
#include <thread>

const auto g_animations = std::array{
    nc::asset::SkeletalAnimation{},
    nc::asset::SkeletalAnimation{}
};

const auto g_animPaths = std::array{
    std::string{"animation1.nca"},
    std::string{"animation2.nca"}
};

const auto g_animId1 = nc::utility::Fnv1a(g_animPaths[0]);
const auto g_animId2 = nc::utility::Fnv1a(g_animPaths[1]);

const auto g_boneData = std::array{
    nc::asset::BonesData{},
    nc::asset::BonesData{}
};

const auto g_meshPaths = std::array{
    std::string{"mesh1.nca"},
    std::string{"mesh2.nca"}
};

const auto g_meshId1 = nc::utility::Fnv1a(g_meshPaths[0]);
const auto g_meshId2 = nc::utility::Fnv1a(g_meshPaths[1]);

TEST(SkeletalAnimationStorageTest, SkeletalAnimationFunctions_tracksLoadedAssets)
{
    auto uut = nc::graphics::SkeletalAnimationStorage{};
    EXPECT_FALSE(uut.HasAnimation(g_animId1));
    EXPECT_FALSE(uut.HasAnimation(g_animId2));
    uut.LoadAnimations(g_animPaths, g_animations);
    EXPECT_TRUE(uut.HasAnimation(g_animId1));
    EXPECT_TRUE(uut.HasAnimation(g_animId2));

    uut.UnloadAnimation(g_animPaths[1]);
    EXPECT_TRUE(uut.HasAnimation(g_animId1));
    EXPECT_FALSE(uut.HasAnimation(g_animId2));

    uut.UnloadAllAnimations();
    EXPECT_FALSE(uut.HasAnimation(g_animId1));
    EXPECT_FALSE(uut.HasAnimation(g_animId2));
}

TEST(SkeletalAnimationStorageTest, BonesFunctions_tracksLoadedAssets)
{
    auto uut = nc::graphics::SkeletalAnimationStorage{};
    EXPECT_FALSE(uut.HasRig(g_meshId1));
    EXPECT_FALSE(uut.HasRig(g_meshId2));
    uut.LoadBones(g_meshPaths, g_boneData);
    EXPECT_TRUE(uut.HasRig(g_meshId1));
    EXPECT_TRUE(uut.HasRig(g_meshId2));

    uut.UnloadBones(g_meshPaths[1]);
    EXPECT_TRUE(uut.HasRig(g_meshId1));
    EXPECT_FALSE(uut.HasRig(g_meshId2));

    uut.UnloadAllBones();
    EXPECT_FALSE(uut.HasRig(g_meshId1));
    EXPECT_FALSE(uut.HasRig(g_meshId2));
}

TEST(SkeletalAnimationStorageTest, LoadOperations_acquireExclusiveLock)
{
    auto uut = nc::graphics::SkeletalAnimationStorage{};

    auto test = [&](auto&& func, auto&&... args)
    {
        std::thread reader([&]() {
            auto lock = uut.AcquireReadLock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto startTime = std::chrono::high_resolution_clock::now();

        // expect this to block for a bit while reader lock is held
        std::thread writer([&]() {
            (uut.*func)(args...);
        });

        writer.join();
        auto endTime = std::chrono::high_resolution_clock::now();
        reader.join();

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    };

    using namespace nc::graphics;

    // Leaving a lot of headroom on expected time to avoid false positives. Loads are trivial, so 50ms should be plenty.
    const auto expected = 50;
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::LoadAnimations, g_animPaths, g_animations));
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::UnloadAnimation, g_animPaths[0]));
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::UnloadAllAnimations));
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::LoadBones, g_meshPaths, g_boneData));
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::UnloadBones, g_meshPaths[0]));
    EXPECT_LE(expected, test(&SkeletalAnimationStorage::UnloadAllBones));
}
