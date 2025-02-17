#include "jolt/JoltApiFixture.inl"
#include "ncengine/physics/Vehicle.h"

#include "physics/jolt/ConstraintFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

class VehicleTest : public JoltApiFixture
{
    protected:
        VehicleTest()
            : constraintFactory{joltApi.physicsSystem}
        {
        }

        ~VehicleTest()
        {
            constraints.clear();
            for (auto body : bodies)
            {
                DestroyBody(body);
            }
        }

    public:
        nc::physics::ConstraintFactory constraintFactory;
        std::vector<JPH::Body*> bodies;
        std::vector<JPH::Ref<JPH::VehicleConstraint>> constraints;

        auto CreateVehicle(nc::VehicleInfo&& info) -> nc::Vehicle
        {
            auto body = CreateBody();
            auto constraint = constraintFactory.MakeVehicleConstraint(info, *body);
            auto vehicle = nc::Vehicle{std::move(info), constraint.GetPtr(), static_cast<nc::VehicleId>(constraints.size())};
            bodies.push_back(body);
            constraints.push_back(std::move(constraint));
            return vehicle;
        }

        auto GetConstraint(const nc::Vehicle& vehicle) -> JPH::VehicleConstraint*
        {
            return static_cast<JPH::VehicleConstraint*>(vehicle.GetHandle());
        }

        auto GetController(const nc::Vehicle& vehicle) -> JPH::WheeledVehicleController*
        {
            return static_cast<JPH::WheeledVehicleController*>(GetConstraint(vehicle)->GetController());
        }
};

TEST_F(VehicleTest, InputFunctions_setValues)
{
    auto uut = CreateVehicle(nc::VehicleInfo{});

    uut.SetThrottle(0.1f);
    uut.SetSteering(0.2f);
    uut.SetBrake(0.3f);
    uut.SetHandBrake(0.4f);
    EXPECT_FLOAT_EQ(0.1f, uut.GetThrottle());
    EXPECT_FLOAT_EQ(0.2f, uut.GetSteering());
    EXPECT_FLOAT_EQ(0.3f, uut.GetBrake());
    EXPECT_FLOAT_EQ(0.4f, uut.GetHandBrake());

    uut.SetInput(0.01f, 0.02f, 0.03f, 0.04f);
    EXPECT_FLOAT_EQ(0.01f, uut.GetThrottle());
    EXPECT_FLOAT_EQ(0.02f, uut.GetSteering());
    EXPECT_FLOAT_EQ(0.03f, uut.GetBrake());
    EXPECT_FLOAT_EQ(0.04f, uut.GetHandBrake());

    uut.HardStopEngine();
    EXPECT_FLOAT_EQ(0.0f, uut.GetThrottle());
    EXPECT_FLOAT_EQ(0.0f, uut.GetSteering());
    EXPECT_FLOAT_EQ(0.0f, uut.GetBrake());
    EXPECT_FLOAT_EQ(0.0f, uut.GetHandBrake());
}

TEST_F(VehicleTest, GetCountFunctions_returnExpectedCounts)
{
    auto uut = CreateVehicle(nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{},
            nc::WheelAssembly{.leftWheel = nc::WheelMount::MakeDisabled()},
            nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()},
            nc::WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()}
        }
    });

    EXPECT_EQ(4, uut.GetAssemblyCount());
    EXPECT_EQ(6, uut.GetWheelCount());
    EXPECT_EQ(3, uut.GetDifferentialCount());
}

TEST_F(VehicleTest, NotifyModifyEngine_updatesConstraint)
{
    auto uut = CreateVehicle(nc::VehicleInfo{.engine = nc::VehicleEngine{.maxTorque = 100.0f}});
    const auto& actualEngine = GetController(uut)->GetEngine();

    uut.GetEngine().maxTorque = 0.0f;
    EXPECT_FLOAT_EQ(100.0f, actualEngine.mMaxTorque);
    uut.NotifyModifyEngine();
    EXPECT_FLOAT_EQ(0.0f, actualEngine.mMaxTorque);
}

TEST_F(VehicleTest, NotifyModifyTransmission_updatesConstraint)
{
    auto uut = CreateVehicle(nc::VehicleInfo{
        .transmission = nc::VehicleTransmission{
            .gears = {1.0f, 2.0f}
        }
    });

    const auto& actualTransmission = GetController(uut)->GetTransmission();

    uut.GetTransmission().gears.pop_back();
    EXPECT_EQ(2, actualTransmission.mGearRatios.size());
    uut.NotifyModifyTransmission();
    EXPECT_EQ(1, actualTransmission.mGearRatios.size());
    EXPECT_FLOAT_EQ(1.0f, actualTransmission.mGearRatios.at(0));
}

TEST_F(VehicleTest, NotifyModifyWheelAssembly_updatesConstraint)
{
    auto uut = CreateVehicle(nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{
                .rightWheel = nc::WheelMount::MakeDisabled(),
                .wheelSpec = nc::WheelSpec{
                    .radius = 0.5f
                }
            }
        }
    });

    const auto& actualWheels = GetConstraint(uut)->GetWheels();

    uut.GetWheelAssemblies()[0].wheelSpec.radius = 1.0f;
    EXPECT_FLOAT_EQ(0.5f, actualWheels.at(0)->GetSettings()->mRadius);
    uut.NotifyModifyWheelAssembly(0);
    EXPECT_FLOAT_EQ(1.0f, actualWheels.at(0)->GetSettings()->mRadius);
}

TEST_F(VehicleTest, RemoveWheelAssembly_updatesConstraint)
{
    auto uut = CreateVehicle(nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{},
            nc::WheelAssembly{
                .suspension = {
                    .antiRollBarStiffness = 100.0f
                }
            }
        }
    });

    const auto& actualWheels = GetConstraint(uut)->GetWheels();
    const auto& actualRollBars = GetConstraint(uut)->GetAntiRollBars();
    const auto& actualDifferentials = GetController(uut)->GetDifferentials();

    EXPECT_EQ(4, actualWheels.size());
    EXPECT_EQ(1, actualRollBars.size());
    EXPECT_EQ(2, actualDifferentials.size());
    uut.RemoveWheelAssembly(1);
    EXPECT_EQ(2, actualWheels.size());
    EXPECT_EQ(0, actualRollBars.size());
    EXPECT_EQ(1, actualDifferentials.size());
}

TEST_F(VehicleTest, RemoveWheelAssembly_lastDifferential_throws)
{
    auto uut = CreateVehicle(nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{},
            nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()}
        }
    });

    EXPECT_THROW(uut.RemoveWheelAssembly(0), nc::NcError);
}

TEST_F(VehicleTest, Enable_togglesState)
{
    auto uut = CreateVehicle(nc::VehicleInfo{});
    EXPECT_TRUE(uut.IsEnabled());
    uut.Enable(false);
    EXPECT_FALSE(uut.IsEnabled());
    uut.Enable(true);
    EXPECT_TRUE(uut.IsEnabled());
}
