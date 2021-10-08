#pragma once

#include <string>
#include <vector>

namespace nc
{
    /** Loading assets creates an internal mapping between the asset path and data.
     *  Duplicate loads are ignored. Objects that use assets do not load them on
     *  demand. Assets must be loaded before objects dependent on them are created. */

    /** Load an audio file.
     *  Supported file types: .wav */
    void LoadSoundClipAsset(const std::string& path);
    
    /** Load a hull collider from a file.
     *  Supported file types: .nca */
    void LoadConvexHullAsset(const std::string& path);

    /** Load a concave collider from a file.
     *  Supported file types: .nca */
    void LoadConcaveColliderAsset(const std::string& path);

    /** Load a mesh from a file.
     *  Supported file types: .nca */
    void LoadMesh(const std::string& path);

    /** Load meshes from multiple files. Prefer this over mulitple
     *  LoadMesh calls as GPU buffers will only be reallocated once.
     *  Supported file types: .nca */
    void LoadMeshes(const std::vector<std::string>& paths);
}