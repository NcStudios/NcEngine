add_subdirectory(external/enet/src)
add_subdirectory(external/imgui/src)
add_subdirectory(engine)

if(NOT NC_PROD_BUILD)
    add_subdirectory(editor)
endif()
