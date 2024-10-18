### DiligentEngineLinuxInt_tests Tests ###
add_graphics_test(DiligentEngineLinuxInt_tests)

target_sources(DiligentEngineLinuxInt_tests
    PRIVATE
        DiligentEngineLinuxInt_tests.cpp
        ${NC_SOURCE_DIR}/config/Config.cpp
)
