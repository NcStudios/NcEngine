#Nce Functional Tests


add_executable(Nce_integration_tests 
                   ${ROOT_DIR}/test/integration/Nce_integration_tests.cpp 
                   ${ROOT_DIR}/nc/NCE.cpp
                   ${ROOT_DIR}/nc/Entity.cpp
                   ${ROOT_DIR}/nc/component/Component.cpp
                   ${ROOT_DIR}/nc/component/Transform.cpp
                   ${ROOT_DIR}/nc/component/Camera.cpp
                   ${ROOT_DIR}/nc/views/EntityView.cpp
                   ${ROOT_DIR}/nc/math/Vector3.cpp
                   ${ROOT_DIR}/nc/time/NCTime.cpp
                   ${ROOT_DIR}/nc/input/Input.cpp
                   ${ROOT_DIR}/nc/ProjectSettings.cpp
                   ${ROOT_DIR}/nc/engine/Engine.cpp
                   ${ROOT_DIR}/nc/engine/LightSystem.cpp
                   ${ROOT_DIR}/nc/engine/CollisionSystem.cpp
                   ${ROOT_DIR}/nc/scene/SceneManager.cpp
                   ${ROOT_DIR}/nc/graphics/Model.cpp
                   ${ROOT_DIR}/nc/graphics/d3dresource/GraphicsResource.cpp
                   ${ROOT_DIR}/nc/utils/objloader/ObjLoader.cpp
                   ${ROOT_DIR}/project/scenes/InitialScene.cpp
                   ${ROOT_DIR}/project/components/Head.cpp
                   ${ROOT_DIR}/project/components/CamController.cpp
                   ${ROOT_DIR}/project/components/Timer.cpp)

if(DEBUG_BUILD)
  #add_definitions(-DNC_DEBUG)
  target_compile_options(Nce_integration_tests PUBLIC ${GCC_DEBUG_FLAGS})
endif()

target_compile_options(Nce_integration_tests PUBLIC ${GCC_WARNING_FLAGS})

target_link_libraries(Nce_integration_tests PUBLIC ${WIN32_LINK_FLAGS} gtest imgui-lib)

target_include_directories(Nce_integration_tests
                             PUBLIC
                               ${NC_COMMON_DIRS}
                               ${PROJECT_INCLUDE_DIRS}
                               ${ROOT_DIR}/external/gtest/googletest/include
)

#target_link_libraries()