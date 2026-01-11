/**
 * @file Scene.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncmath/Vector.h"

#include <filesystem>
#include <memory>

namespace nc
{
class Registry;

constexpr float MinExtentDimension = 1.0f;
class Scene
{
    public:
        Scene(Vector3 extents = Vector3{100.0f, 100.0f, 100.0f})
        : m_extents{Vector3{std::max(MinExtentDimension, extents.x), std::max(MinExtentDimension, extents.y), std::max(MinExtentDimension, extents.z)}}
        {}

        virtual ~Scene() = default;

        /** 
         * @brief Handle to put any logic to occur on scene load.
         */
        virtual void Load(ecs::Ecs world, ModuleProvider modules) = 0;

        /** 
         * @brief Handle to put any logic to occur on scene unload.
         */
        virtual void Unload() {}

        /**
         * @brief Get the scene extents, which are used to determine lighting bounds.
         * @return a Vector3 representing the x, y, and z extents of the scene.
         */
        auto GetExtents() const noexcept -> const Vector3& { return m_extents; };

    private:
        Vector3 m_extents;
};
} // namespace nc
