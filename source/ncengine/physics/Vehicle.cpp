#include "ncengine/physics/Vehicle.h"

#include "jolt/ConstraintFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"
#include "ncutility/NcError.h"

namespace
{
auto ToConstraint(nc::VehicleHandle handle) -> JPH::VehicleConstraint*
{
    return static_cast<JPH::VehicleConstraint*>(handle);
}

auto ToController(nc::VehicleHandle handle) -> JPH::WheeledVehicleController*
{
    auto constraint = ToConstraint(handle);
    return static_cast<JPH::WheeledVehicleController*>(constraint->GetController());
}
} // anonymous namespace

namespace nc
{
auto Vehicle::GetThrottle()  const -> float { return  ToController(m_handle)->GetForwardInput();   }
auto Vehicle::GetSteering()  const -> float { return -ToController(m_handle)->GetRightInput();     }
auto Vehicle::GetBrake()     const -> float { return  ToController(m_handle)->GetBrakeInput();     }
auto Vehicle::GetHandBrake() const -> float { return  ToController(m_handle)->GetHandBrakeInput(); }

void Vehicle::SetThrottle(float throttle) { ToController(m_handle)->SetForwardInput(throttle); }
void Vehicle::SetSteering(float steering) { ToController(m_handle)->SetRightInput(-steering);  }
void Vehicle::SetBrake(float brake)       { ToController(m_handle)->SetBrakeInput(brake);      }
void Vehicle::SetHandBrake(float brake)   { ToController(m_handle)->SetHandBrakeInput(brake);  }

void Vehicle::SetInput(float throttle, float steering, float brake, float handBrake)
{
    ToController(m_handle)->SetDriverInput(throttle, -steering, brake, handBrake);
}

void Vehicle::HardStopEngine()
{
    auto controller = ToController(m_handle);
    controller->SetDriverInput(0.0f, 0.0f, 0.0f, 0.0f);
    controller->GetEngine().SetCurrentRPM(0.0f);
    controller->GetTransmission().Set(0, 1.0f);
}

auto Vehicle::GetWheelCount() const -> size_t
{
    return std::ranges::fold_left(
        m_info.wheelAssemblies,
        0ull,
        [](size_t count, const auto& assembly) {
            return count + assembly.WheelCount();
        }
    );
}

auto Vehicle::GetDifferentialCount() const -> size_t
{
    return std::ranges::count_if(
        m_info.wheelAssemblies,
        &Differential::IsEnabled,
        &WheelAssembly::differential
    );
}

void Vehicle::SetMaxRollAngle(float angle)
{
    m_info.orientation.maxRollAngle = angle;
    ToConstraint(m_handle)->SetMaxPitchRollAngle(angle);
}

void Vehicle::NotifyModifyEngine()
{
    physics::SetVehicleEngineSettings(
        m_info.engine,
        ToController(m_handle)->GetEngine()
    );
}

void Vehicle::NotifyModifyTransmission()
{
    // If gear count changed, need to make sure we're still in a valid gear.
    // note: 0 is neutral so gears arrays are indexed from 1
    auto& transmission = ToController(m_handle)->GetTransmission();
    const auto currentGear = transmission.GetCurrentGear();
    const auto& settings = m_info.transmission;
    if (currentGear > 0 && currentGear > settings.gears.size())
    {
        transmission.Set(
            static_cast<int>(settings.gears.size()),
            transmission.GetClutchFriction()
        );
    }
    else if (currentGear < 0 && -currentGear > settings.reverseGears.size())
    {
        transmission.Set(
            -static_cast<int>(settings.reverseGears.size()),
            transmission.GetClutchFriction()
        );
    }

    physics::SetVehicleTransmissionSettings(settings, transmission);
}

void Vehicle::AddWheelAssembly(const WheelAssembly& info)
{
    m_info.wheelAssemblies.push_back(info);
    return physics::AddWheelAssembly(
        m_info.wheelAssemblies.back(),
        *ToConstraint(m_handle),
        *ToController(m_handle)
    );
}

void Vehicle::RemoveWheelAssembly(size_t assemblyIndex)
{
    NC_ASSERT(
        !m_info.wheelAssemblies.at(assemblyIndex).IsPowered() ||
        GetDifferentialCount() > 1,
        "At least one wheel with one differential is required"
    );

    physics::RemoveWheelAssembly(
        assemblyIndex,
        m_info.wheelAssemblies,
        *ToConstraint(m_handle),
        *ToController(m_handle)
    );
}

void Vehicle::NotifyModifyWheelAssembly(size_t index)
{
    physics::ModifyWheelAssembly(
        m_info.wheelAssemblies.at(index),
        *ToConstraint(m_handle),
        *ToController(m_handle)
    );
}

void Vehicle::Enable(bool enabled)
{
    if (!enabled)
    {
        HardStopEngine();
    }

    SetInput(0.0f, 0.0f, 0.0f, 0.0f);
    ToConstraint(m_handle)->SetEnabled(enabled);
}

auto Vehicle::IsEnabled() const -> bool
{
    return ToConstraint(m_handle)->GetEnabled();
}
} // namespace nc
