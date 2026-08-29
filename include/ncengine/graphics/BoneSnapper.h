/**
 * @file BoneSnapper.h
 * @copyright Jaremie Romer and McCallister Romer 2026
 */

 #pragma once

 #include "ncengine/ecs/Entity.h"

#include <string>

/**
 * Strategy:
 * [x] Add component that gets the name of the target bone to snap to and the entity with the SkinnedMesh that contains the bones
 * [-] Build the mapping between bone name and final animated transform for the bone (could be improved)
 *  For each BoneSnapper, on update, we need to:
 * [x] Validate the BoneSnapper's target entity has a SkinnedMesh
 * [x] Create wiring between SkinnedMesh ID and bone ID
 * [x] Validate that the SkinnedMesh has a bone with the given boneName 
 * [] Apply the bone's transformation matrix to the Transform
 * [] Commit changes to the transforms.
 * 
 * Todos:
 * Write unit tests
 * [] Target entity does not have a skinned mesh (skip that entity)
 * [] Target entity does not have that bone (skip that entity)
 * [] Bone Names are out of sync with Bone Transforms (throw) (BoneCache)
 * [] Entity's transform is moved by the correct transformation matrix
 * [] Entity with children - children's transform is unaffected 
 * 
 * Add editor widget
 * [] Select an target entity
 * [] Enumerate the bone names on that entity in a selection dropdown.
 * 
 */


 namespace nc
 {
    struct BoneSnapper
    {
        std::string boneName;
        Entity target;
    };
 } // namespace nc