# TODO: make issue for this, i'd rather have a Cmake opt for
# enabling editor that we can check here
if (NC_PROD_BUILD)
    add_subdirectory(stub)
else()
    add_subdirectory(impl)
endif()
