#include "gtest/gtest.h"
#include "ecs/component/AttachmentGroup.h"

using namespace nc;

struct FakeAttachment : public StateAttachment
{
    FakeAttachment(Entity entity)
        : StateAttachment{entity}
    {
    }
};

constexpr auto TestEntity = Entity{0, 0, 0};

TEST(AttachmentGroup_unit_tests, Add_ValidCall_ConstructsObject)
{
    auto group = AttachmentGroup{TestEntity};
    auto ptr = group.Add<FakeAttachment>(TestEntity);
    EXPECT_EQ(ptr->ParentEntity(), TestEntity);
}

TEST(AttachmentGroup_unit_tests, Add_ReplaceAfterRemove_ConstructsObject)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    group.Remove<FakeAttachment>();
    group.CommitStagedAttachments();
    auto ptr = group.Add<FakeAttachment>(TestEntity);
    EXPECT_EQ(ptr->ParentEntity(), TestEntity);
}

TEST(AttachmentGroup_unit_tests, Add_DoubleCall_Throws)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    EXPECT_THROW(group.Add<FakeAttachment>(TestEntity), std::runtime_error);
}

TEST(AttachmentGroup_unit_tests, Remove_ComponentExists_Removes)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    group.Remove<FakeAttachment>();
    group.CommitStagedAttachments();
    auto actual = group.Contains<FakeAttachment>();
    EXPECT_FALSE(actual);
}

TEST(AttachmentGroup_unit_tests, Remove_ComponentDoesNotExist_Throws)
{
    auto group = AttachmentGroup{TestEntity};
    EXPECT_THROW(group.Remove<FakeAttachment>(), std::runtime_error);
}

TEST(AttachmentGroup_unit_tests, Remove_DoubleCall_Throws)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    group.Remove<FakeAttachment>();
    group.CommitStagedAttachments();
    EXPECT_THROW(group.Remove<FakeAttachment>(), std::runtime_error);
}

TEST(AttachmentGroup_unit_tests, Contains_Exists_ReturnsTrue)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    auto actual = group.Contains<FakeAttachment>();
    EXPECT_TRUE(actual);
}

TEST(AttachmentGroup_unit_tests, Contains_ExistsStaged_ReturnsTrue)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    auto actual = group.Contains<FakeAttachment>();
    EXPECT_TRUE(actual);
}

TEST(AttachmentGroup_unit_tests, Contains_DoesNotExist_ReturnsFalse)
{
    auto group = AttachmentGroup{TestEntity};
    auto actual = group.Contains<FakeAttachment>();
    EXPECT_FALSE(actual);
}

TEST(AttachmentGroup_unit_tests, Contains_AfterRemoved_ReturnsFalse)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    group.Remove<FakeAttachment>();
    group.CommitStagedAttachments();
    auto actual = group.Contains<FakeAttachment>();
    EXPECT_FALSE(actual);
}

TEST(AttachmentGroup_unit_tests, Get_Exists_ReturnsPointer)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    auto* ptr = group.Get<FakeAttachment>();
    EXPECT_NE(ptr, nullptr);
}

TEST(AttachmentGroup_unit_tests, Get_ExistsStaged_ReturnsPointer)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    auto* ptr = group.Get<FakeAttachment>();
    EXPECT_NE(ptr, nullptr);
}

TEST(AttachmentGroup_unit_tests, Get_DoesNotExist_ReturnsNull)
{
    auto group = AttachmentGroup{TestEntity};
    auto* ptr = group.Get<FakeAttachment>();
    EXPECT_EQ(ptr, nullptr);
}

TEST(AttachmentGroup_unit_tests, Get_CallAfterRemoved_ReturnsNull)
{
    auto group = AttachmentGroup{TestEntity};
    group.Add<FakeAttachment>(TestEntity);
    group.CommitStagedAttachments();
    group.Remove<FakeAttachment>();
    group.CommitStagedAttachments();
    auto* ptr = group.Get<FakeAttachment>();
    EXPECT_EQ(ptr, nullptr);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}