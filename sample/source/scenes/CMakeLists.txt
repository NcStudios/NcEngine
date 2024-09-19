target_sources(${NC_SAMPLE_EXE}
    PRIVATE
        Benchmarks.cpp
        GraphicsTest.cpp
        SmokeTest.cpp
)

if(NC_USE_JOLT)
    target_sources(${NC_SAMPLE_EXE}
        PRIVATE
            PhysicsTest2.cpp
    )
else()
    target_sources(${NC_SAMPLE_EXE}
        PRIVATE
            PhysicsTest.cpp
    )
endif()
