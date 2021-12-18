# Gets added in debug build; removing so tests don't have to link against imgui
remove_definitions(-DNC_EDITOR_ENABLED)

add_subdirectory(alloc)
add_subdirectory(assets)
add_subdirectory(ecs)
add_subdirectory(network)
add_subdirectory(task)