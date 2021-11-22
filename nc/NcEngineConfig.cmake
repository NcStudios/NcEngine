include(CMakeFindDependencyMacro)
find_dependency(Vulkan REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/NcEngineTargets.cmake")