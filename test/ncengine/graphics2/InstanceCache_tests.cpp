#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/InstanceCache.h"

TEST(InstanceCacheTest, Foo)
{
    auto uut = nc::graphics::InstanceCache2{};

    uut.AddInstance(
        0, 0,
        nc::MaterialPass::Shadow,
        nc::asset::MeshView{
            .id = 0
        }
    );
}
