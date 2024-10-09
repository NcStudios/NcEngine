# TODO: #465 Add CMake option for NC_EDITOR_ENABLED
if (NC_PROD_BUILD)
    add_subdirectory(stub)
else()
    add_subdirectory(impl)
endif()
