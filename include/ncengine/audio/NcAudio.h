/**
 * @file NcAudio.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/module/Module.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/utility/Signal.h"

#include <limits>

namespace nc
{
namespace config
{
struct AudioSettings;
} // namespace config

namespace audio
{
/** @brief Id representing a system's default audio device. */
constexpr auto DefaultDeviceId = std::numeric_limits<uint32_t>::max();

/** @brief Id representing a null audio device. */
constexpr auto InvalidDeviceId = DefaultDeviceId - 1u;

/** @brief The name and device id of an available audio device. */
struct AudioDevice
{
    std::string name;
    uint32_t id;
};

/**
 * @brief Audio module interface.
 * 
 * Tasks:
 *   Process Audio
 *     Runs During: ExecutionPhase::Free
 *     Component Access:
 *       Write: AudioSource
 *       Read: Transform
*/
struct NcAudio : public Module
{
    explicit NcAudio() noexcept
        : Module{NcAudioId} {}

    /**
     * @brief Register an object to be the reference point audio.
     * @param entity An Entity to use as the listener.
     * 
     * A listener must be registered for the audio module to operate. A listener
     * should not be destroyed while it is registered, except on a scene change.
     * A new Entity or Entity::Null() may be passed to unregister the current
     * listener.
     */
    virtual void RegisterListener(Entity entity) noexcept = 0;

    /**
     * @brief Get the current stream time.
     * @return The time in seconds since the stream was started.
     * 
     * The stream time is an approximation of the number of seconds the stream has
     * been playing based on the number of buffers processed.
     */
    virtual auto GetStreamTime() const noexcept -> double = 0;

    /**
     * @brief Set the stream time.
     * @param time The time in seconds to overwrite the stream time with. Must be >= 0.
    */
    virtual void SetStreamTime(double time) noexcept = 0;

    /**
     * @brief Get a list of available output devices.
     * @return A vector of AudioDevice objects.
     * 
     * This returned list is not static, and is subject to change upon device
     * connection or disconnection. Devices that do not meet internal usage 
     * requirements are excluded from the list.
     */
    virtual auto EnumerateOutputDevices() noexcept -> std::vector<AudioDevice> = 0;

    /**
     * @brief Get the active output device.
     * @return A const reference to the active AudioDevice.
     * 
     * If no device is in use, the returned value will have an id of InvalidDeviceId.
     */
    virtual auto GetOutputDevice() const noexcept -> const AudioDevice& = 0;

    /**
     * @brief Set the active output device.
     * @param deviceId The id of the preferred device.
     * @return true if any device was successfuly selected.
     * 
     * If the operation fails using the provided id, an attempt will be made to fallback
     * to another suitable device, preferring the system default.
     */
    virtual auto SetOutputDevice(uint32_t deviceId = DefaultDeviceId) noexcept -> bool = 0;

    /**
     * @brief Get the signal for device change events.
     * @return A reference to the signal managing device change events.
    */
    virtual auto OnChangeOutputDevice() noexcept -> Signal<const AudioDevice&>& = 0;
};

/** @brief Build an NcAudio module instance. */
auto BuildAudioModule(const config::AudioSettings& settings, ecs::ExplicitEcs<Entity, Transform, AudioSource> gameState) -> std::unique_ptr<NcAudio>;
} // namespace audio
} // namespace nc
