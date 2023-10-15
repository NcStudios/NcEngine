# add_executable(nc-editor
#     Main.cpp
#     EditorScene.cpp
#     NcEditor.cpp
# )

# target_compile_options(nc-editor
#     PUBLIC
#         ${NC_COMPILER_FLAGS}
# )

# target_compile_definitions(nc-editor
#     PUBLIC
#         ${NC_COMPILE_DEFINITIONS}
# )

# target_include_directories(nc-editor
#     PRIVATE
#         ${PROJECT_SOURCE_DIR}/source/editor
#         ${NC_INCLUDE_DIR}
#         ${NC_EXTERNAL_DIR}
# )

# target_link_libraries(nc-editor
#     PRIVATE
#         ${NC_ENGINE_LIB}
# )

# # add_subdirectory(config)
# # add_subdirectory(ui)

# install(TARGETS     nc-editor
#         DESTINATION editor
# )
