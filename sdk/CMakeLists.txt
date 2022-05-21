add_subdirectory(external/enet/src)
add_subdirectory(external/imgui/src)
add_subdirectory(engine)

if(NOT NC_PROD_BUILD)
    add_subdirectory(editor)
    add_subdirectory(sample)
endif()

if(NC_BUILD_TESTS)
    add_subdirectory(test)
endif()
