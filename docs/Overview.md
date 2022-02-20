Overview
========

## Contents
* [Entities](#overview)
* [Components](#components)
* [Registry](#registry)
* [Views](#views)
* [Scenes](#scenes)
* [Assets](#assets)
* [UI](#ui)
* [Config File](#config)

## Entities
------------
Conceptually, Entities are the objects in a game: a player or a building, for example. Functionally, however, they are handles used to identify the set of internal objects that comprise the game object. An Entity representing a player, for instance, might map to a Transform, MeshRenderer, and Collider. It is these objects that actually describe what the player is and how it behaves, while the Entity forms an association between them. Because of this, they are cheap to copy and pass around as needed.

Entities are made up of an index, layer, and flags. 

### Index
An entity's index is determined and used internally. It can be useful to create a 'null' Entity. This can be done through default construction or with `Entity::Null()`. The resulting Entity will have an index of `std::numeric_limits<uint32_t>::max()`.

### Layers
Layers are underused and are reserved for future use.

### Flags
* `Entity::Flags::Static` Specifies an Entity will not be moved after construction. This is used for various optimizations and is required in certain cases ([ConcaveCollider](EngineComponents.md#concavecollider)).
* `Entity::Flags::Persistent` Persistent entities and their attached components are not destroyed on scene changes.
* `Entity::Flags::NoCollisionNotifications` Attached components will not receive collision/trigger events. This can improve performance in scenes with many colliders.

Entities do not inherit flags from their parents.

When creating an Entity through the [registry](#registry), an EntityInfo struct is required, which has more information than is actually stored in the Entity. This is because the registry automatically adds a [Transform](EngineComponents.md#transform) and [Tag](EngineComponents.md#tag) to each new Entity, initialized with data from the EntityInfo.

## Components
-------------
Components describe the behavior and properties of objects in a game. An Entity may have at most one Component of each type. A number of components are provided by NcEngine which are described [here](EngineComponents.md).

Components must be added and removed through the Registry. They are expected to be move-only types.

### Pooled Component
Pooled components are intended for types that are numerous and processed in bulk. They are stored contiguously by type and have fixed size memory overhead for maintaining constant random access time. User-defined pooled components should derive from [ComponentBase](../nc/include/ecs/component/Component.h). Also, `Registry::RegisterComponentType<T>()` needs to be called once before interacting with the type in the Registry. This is done internally for all engine types.

### Free Component
Free components should be used for types that appear in small numbers - an input handler, for example. They also have the advantage of being pointer stable. Free components are grouped according to the Entities they belong to and do not have the additional memory overhead of pooled components. This, however, results in random access times that are linear in terms of the number of free components attached to an Entity. User-defined free components should derive from [FreeComponent](../nc/include/ecs/component/Component.h).

### Systems, Callbacks, and Storage Policies
Pooled components usually want to be processed as a whole or in batches by an external system. This can be set up in whatever way best suits the scenario. A simple solution is to implement the systems as FreeComponents attached to a single persistent Entity, which can then be invoked by a [FrameLogic or FixedLogic](EngineComponents.md#logic) component.

Callbacks can be set in the Registry to receive on add/remove notifications for a particular type. First, a specialization of storage_policy must be provided specifying which callbacks will be used. Then `Registry::RegisterOnAddCallback<T>(cb)` and/or `Registry::RegisterOnRemoveCallback<T>(cb)` can be used. The callback signatures are `void Func(T&)` for additions and `void Func(Entity)` for removals.

## Registry
-----------
The registry contains storage for all entities and components and manages the associations between them. In order for objects of these types to be recognized by NcEngine, they must be created through one of the registry's Add member functions. There are also functions for retreiving, removing, and viewing ranges of objects, among other things.

A pointer to the registry may be retrieved from the NcEngine object, which is passed to each scene's Load() function. This pointer will remain valid until the engine is shutdown, so it can be passed around and stored as needed.

Generally, don't store values from the registry across frames, except for Entities. Calls to Add, Remove, ViewGroup, Sort and ReserveHeadroom for a type T may invalidate any existing span\<T\> or pointers/references to other Ts except:
* if T is derived from FreeComponent, which is guaranteed to be pointer stable.
* Add\<T\> will not invalidate a span\<T\> until the end of the current frame. It is possible to add while iterating a collection.
* Remove\<T\> will not invalidate the subrange [ begin(), T's position ) of a span\<T\>. It is possible to remove while reverse iterating a collection.

When adding and removing objects, certain operations are delayed until later in the frame. Staging pools are used, which are merged after all potentially modifying tasks (FrameUpdate, FixedUpdate, etc.) for that frame have have completed, but before rendering.

When adding an Entity or Component:
* Get/Contains will work as expected.
* Returned ranges will not immediately include the new item.

When removing an Entity:
* The Entity and its children will not be present in returned ranges, nor will they be found with Get/Contains.
* Their associated Components will persist until merging is complete. This prevents things like removing an Entity inside FrameUpdate() from causing issues.

When removing a Component:
* It is destroyed before returning from Remove and changes are immediately reflected in returned ranges.

## Views
--------
Views represent ranges of registry data. They come in two forms, `view<viewable>` and `multi_view<viewable...>`, where `viewable` is either `Entity` or models `PooledComponent`. They each provide their own iterator types, and can be used in range-based for loops and the like. Example usage:

```cpp
for(auto& [t, u, v] : multi_view<T, U, V>{myRegistry})
{
    /** ... */
}
```

A `view<T>` will always be a contiguous range, either of all the active entities or all *committed* components of type T. This may exclude components added during the current frame, in accordance with the registry's staging behavior.

A `multi_view<Ts...>` allows filtering the registry for all entities with the specified component list and viewing them as component sets. There is a mix of contiguous and random access used internally, which makes their cost context-dependent:
* The number of registry reads during a full iteration is at *most* the number of components in the smallest pool mutiplied by the number of component types being viewed.
    * Consider the example code above, where there are 10 Ts, 20 Us, and 20 Vs. The entire iteration querries storage for T 10 times (sequential), U 10 times (random), and V at most 10 times (random).
* The above point hints that some short-circuiting is possible. This can be taken advantage of by ordering template arguments by increasing component count (or an educated guess).
* The fragmentation of the individual pools relative to one another determines the cost of random accesses. Sorting the pools beforehand minimizes cache misses, but comes with its own cost.

## Scenes
---------
Scenes manage initialization of the game world. Scenes should derive from the abstract base Scene and overload two functions:

```cpp
/** Scene.h */
virtual void Load(NcEngine* engine) = 0;
virtual void Unload() = 0;
```

A scene will do most of its work during Load: adding to the registry, loading assets, etc. A scene does not have to clear or remove anything from the registry in Unload(). In some cases, Unload() will not have to do anything.

Scenes may hold any required data as members and are guaranteed to have stable addresses.

Scenes may be changed through the SceneSystem interface:
```cpp
/** Scene.h */
virtual void SceneSystem::ChangeScene(std::unique_ptr<Scene> scene);
```

This may be called from anywhere, but the scene change will not happen until all running tasks have finished. The scene change process is as follows:
1. oldScene->Unload()
2. Destroy oldScene
3. Clear registry
4. newScene->Load(registry)

## Assets
---------
Assets are the resources used by some built in components. The components that use assets are:

* AudioSource: expects a path to a sound file.
* Collider(Hull): expects a path to a convex hull mesh.
* ConcaveCollider: expects a path to concave mesh.
* MeshRenderer: expects a path to a mesh and a material wich requires three paths to textures.
* Skybox(not a component): expects a path to a skybox asset (.nca).

Components do not load or own the assets they use, but instead obtain a view over the data. This means an asset should be loaded before attempting to create a component that depends on it. For example, if you want to create an AudioSource that plays "my_sound.wav", you must have previously loaded "my_sound.wav" by calling LoadAudioClipAsset("my_sound.wav"). The loading functions and supported file types can be found in [Assets.h](../nc/include/Assets.h).

Assets related to 3d meshes(meshes, convex hulls, concave colliders) and skyboxes can only be loaded from .nca files. This is done to move runtime-independent calculations into a preprocessing step. Nca files can be generated from input files using the asset builder (tools/asset_builder/build.exe). Usage can be shown by running './build --h'. When using NcEditor, this process is handled automatically. Adding fbx files to the asset manifest will create the appropriate nca files.

## UI
-----
NcEngine does not have an proper solution for UI. [Dear ImGui](https://github.com/ocornut/imgui) is used internally, and, as a temporary stand-in, a method for hooking into the internal frame is provided:

```cpp
#include "ui/UISystem.h"

/** Implement the required interface */
class MyUI : nc::ui::UIFlexible
{
    public:
        void Draw() override { ... }
        bool IsHovered() const override { ... }
};

/** Elsewhere - engine is a pointer to NcEngine */
MyUI uiInstance;
engine->UI()->Set(&uiInstance)
```

While registered, Draw() will be called when rendering between ImGui::NewFrame() and ImGui::Render(). The intention is simply to inject ImGui functions into the current frame, but Draw() can be as complicated as needed.

This process is unrefined and easy to misuse. Whatever operations a registered UI performs are done during the render step, which is assumed to not modify the registry. It is recomended that registry pointers in UI functions be const-qualified to guard against segfaults.

## Config
--------------
NcEngine reads some settings from a config file upon initialization. A default version can be found [here](../nc/source/config/default_config.ini). Modifications should conform to these rules:

* One key-value pair per line separated with '='.
* Non-data content appears either:
    * As a line comment after a ';' or '#'
    * As a human-readable section tag enclosed in square brackets
* Extraneous whitespace and blank lines should be avoided.

The default asset paths point to assets in the repository. These should be changed to your own directories - the editor defaults to \<project-name>/assets/\<asset-type>. Asset functions operate relative to the paths specified here. The paths themselves should be absolute or relative to your executable.
