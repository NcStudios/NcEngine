Overview
========

## Contents
* [Entities](#overview)
* [Components](#components)
* [Registry](#registry)
* [Scenes](#scenes)
* [Assets](#assets)
* [UI](#ui)
* [Config File](#config)

## Entities
------------
Conceptually, Entities are the objects in a game: a player or a building, for example. Functionally, however, they are handles used to identify the set of internal objects that comprise the game object. An Entity representing a player, for instance, might map to a Transform, MeshRenderer, and Collider. It is these objects that actually describe what the player is and how it behaves, while the Entity forms an association between them. Because of this, they are cheap to copy and pass around as needed.

Entities are made up of an index, layer, and flags. 

### Index
An entitiy's index is determined and used internally. It can be useful to create a 'null' Entity. This can be done through default construction or with Entity::Null(). The resulting Entity will have an index of 2^32 -1.

### Layers
Layers are underused and are reserved for future use.

### Flags
At the moment, the only Entity flag is Entity::Flags::Static. This is used to specify that an Entity will not be moved after construction. This is used for various optimizations and is required in certain cases ([ConcaveCollider](EngineComponents.md#concavecollider)).

When creating an Entity through the [registry](#registry), an EntityInfo struct is required, which has more information than is actually stored in the Entity. This is because the registry automatically adds a [Transform](EngineComponents.md#transform) and [Tag](EngineComponents.md#tag) to each new Entity, initialized with data from the EntityInfo.

## Components
--------------
Components describe the behavior and properties of objects in a game. An Entity may have at most one Component of each type. A number of components are provided by NcEngine which are described [here](EngineComponents.md). 

Most user-created components should derive from [AutoComponent](../nc/include/component/Component.h). This provides a number of virtual functions that can be overriden to hook into the most common events:

```cpp
/** Called on each frame. */
virtual void FrameUpdate(float dt);

/** Called on each tick of the physics system. */
virtual void FixedUpdate();

/** Called on destruction of the associated Entity. */
virtual void OnDestroy();

/** Called on collision/trigger event detection. */
virtual void OnCollisionEnter(Entity other);
virtual void OnCollisionStay(Entity other);
virtual void OnCollisionExit(Entity other);
virtual void OnTriggerEnter(Entity other);
virtual void OnTriggerStay(Entity other);
virtual void OnTriggerExit(Entity other);
```

The most common uses for components are handled by the included types or by adding logic with FrameUpdate(). See [creating a project](CreatingAProject.md) or the [sample project](../project/source) for examples.

Most engine-provided Components are implemented using an Ecs-oriented approach which is, currently, impossible to extend to user-defined types without rebuilding the engine. To do so:
* Create a type derived from ComponentBase that satisfies the Component concept.
* Add the type to registry_type_list (Ecs.h)
* If there is a system associated with the new type, provide a specialization of StoragePolicy enabling add/remove callbacks. These callbacks need to be set in the registry before calling Start().

## Registry
-----------
The registry contains storage for all entities and components and manages the associations between them. In order for objects of these types to be recognized by NcEngine, they must be created through one of the registry's Add member functions. There are also functions for retreiving, removing, and viewing ranges of objects, among other things.

A pointer to the registry is passed to each scene's Load() function using the alias registry_type. This pointer will remain valid until the engine is shutdown, so it can be passed around and stored as needed.

Generally, don't store return values from the registry, except for Entities. Calls to Add, Remove, ViewGroup, and ReserveHeadroom for a type T may invalidate any existing span\<T\> or pointers/references to other Ts except:
* if T is derived from AutoComponent, which is guaranteed to be pointer stable.
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

## Scenes
---------
Scenes manage initialization of the game world. Scenes should derive from the abstract base Scene and overload two functions:

```cpp
/** Scene.h */
virtual void Load(registry_type* registry) = 0;
virtual void Unload() = 0;
```

A scene will do most of its work during Load: adding to the registry, loading assets, etc. A scene does not have to clear or remove anything from the registry in Unload(). In some cases, Unload() will not have to do anything.

Scenes may hold any required data as members and are guaranteed to have stable addresses.

Scenes may be changed using:
```cpp
/** Scene.h */
void Change(std::unique_ptr<Scene> scene);
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

Components do not load or own the assets they use, but instead obtain a view over the data. This means an assets should be loaded before attempting to create a component that depends on it. For example, if you want to create an AudioSource that plays "my_sound.wav", you must have previously loaded "my_sound.wav" by calling LoadSoundClipAsset("my_sound.wav"). The loading functions and supported file types can be found in [Assets.h](../nc/include/Assets.h).

Assets related to 3d meshes can only be loaded from .nca files. This includes meshes, convex hulls, and concave colliders. This is done to move runtime-independent calculations into a preprocessing step. Nca files can be generated from fbx meshes using the asset builder (tools/asset_builder/build.exe). Usage can be shown by running './build --h'. When using NcEditor, this process is handled automatically. Adding fbx files to the asset manifest will create the appropriate nca files.

## UI
-----
NcEngine does not have an proper solution for UI. [Dear ImGui](https://github.com/ocornut/imgui) is used internally, and, as a temporary stand-in, a method for hooking into the internal frame is provided:

```cpp
#include "UI.h"

/** Implement the required interface */
class MyUI : nc::ui::UIFlexible
{
    public:
        void Draw() override { ... }
        bool IsHovered() const override { ... }
};

/** Elsewhere */
MyUI uiInstance;
nc::ui::Set(&uiInstance)
```

While registered, Draw() will be called when rendering between ImGui::NewFrame() and ImGui::Render(). The intention is simply to inject ImGui functions into the current frame, but Draw() can be as complicated as needed.

This process is unrefined and easy to misuse. Whatever operations a registered UI performs are done during the render step, which is assumed to not modify the registry. It is recomended that registry pointers in UI functions be const-qualified to guard against segfaults.

## Config File
--------------
NcEngine reads some settings from a config file upon initialization. A default version can be found [here](../nc/source/config/default_config.ini). Modifications should conform to these rules:

* One key-value pair per line separated with '='.
* Non-data content appears either:
    * As a line comment after a ';' or '#'
    * As a human-readable section tag enclosed in square brackets
* Extraneous whitespace and blank lines should be avoided.
