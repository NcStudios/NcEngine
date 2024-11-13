target_sources(${NC_SAMPLE_EXE}
    PRIVATE
        Benchmarks.cpp
        GraphicsTest.cpp
        SmokeTest.cpp
)

if(NC_USE_DILIGENT)
    target_sources(${NC_SAMPLE_EXE}
        PRIVATE
            PhysicsTest_Diligent.cpp
    )
else()
    target_sources(${NC_SAMPLE_EXE}
        PRIVATE
            PhysicsTest.cpp
    )
endif()
