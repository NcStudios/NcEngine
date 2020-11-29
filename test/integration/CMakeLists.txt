#### Nce Functional Tests ###
# add_executable(Nce_integration_tests 
#     ${ROOT_DIR}/test/integration/Nce_integration_tests.cpp 
#     ${ROOT_DIR}/nc/source/entity/Entity.cpp
#     ${ROOT_DIR}/nc/source/component/Component.cpp
#     ${ROOT_DIR}/nc/source/component/Transform.cpp
#     ${ROOT_DIR}/nc/source/component/Camera.cpp
#     ${ROOT_DIR}/nc/source/config/Config.cpp
#     ${ROOT_DIR}/nc/source/math/Vector3.cpp
#     ${ROOT_DIR}/nc/source/time/NCTime.cpp
#     ${ROOT_DIR}/nc/source/input/Input.cpp
#     ${ROOT_DIR}/nc/source/engine/Engine.cpp
#     ${ROOT_DIR}/nc/source/engine/system/LightSystem.cpp
#     ${ROOT_DIR}/nc/source/engine/system/CollisionSystem.cpp
#     ${ROOT_DIR}/nc/source/graphics/Model.cpp
#     ${ROOT_DIR}/nc/source/graphics/Material.cpp
#     ${ROOT_DIR}/nc/source/graphics/WICTextureLoader.cpp
#     ${ROOT_DIR}/nc/source/graphics/d3dresource/GraphicsResource.cpp
#     ${ROOT_DIR}/nc/source/ui/UI.cpp
#     ${ROOT_DIR}/project/scenes/InitialScene.cpp
#     ${ROOT_DIR}/project/components/CamController.cpp
#     ${ROOT_DIR}/project/components/DebugUtils.cpp
#     ${ROOT_DIR}/project/components/Ship.cpp
#     ${ROOT_DIR}/project/ui/HUD.cpp
#     ${ROOT_DIR}/project/ui/LogUIElement.cpp
#     ${ROOT_DIR}/project/ui/TurnPhaseUIElement.cpp
# )

# target_compile_options(Nce_integration_tests
#     PUBLIC
#         ${GCC_WARNING_FLAGS}
# )

# if(DEBUG_BUILD)
#   target_compile_options(Nce_integration_tests
#       PUBLIC
#           ${GCC_DEBUG_FLAGS}
#   )
# endif()

# target_include_directories(Nce_integration_tests
#     PRIVATE
#         ${NC_INCLUDE_DIRS}
#         ${PROJECT_INCLUDE_DIR}
#         ${ROOT_DIR}/external/gtest/googletest/include
#         ${EXTERNAL_INCLUDE_DIR}
# )

# if(NC_EDITOR_ENABLED)
#     target_link_libraries(Nce_integration_tests
#         PRIVATE
#             ${EXTERNAL_LIB_DIR}/libimgui.a
#     )
# endif()

# target_link_libraries(Nce_integration_tests
#     PRIVATE
#         ${EXTERNAL_LIB_DIR}/windowscodecs.lib
#         ${EXTERNAL_LIB_DIR}/libassimp.dll.a
#         ${WIN32_LINK_FLAGS}
#         gtest
# )

#add_test(Nce_integration_tests Nce_integration_tests)