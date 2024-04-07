add_library(RtAudio
    STATIC
        RtAudio.cpp
)

if(WIN32)
    target_compile_definitions(RtAudio
        PRIVATE
            __WINDOWS_WASAPI__
            -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # RtAudio uses deprecated wchar_t conversion
    )
elseif(APPLE)
    target_compile_definitions(RtAudio
        PRIVATE
            __MACOSX_CORE__
    )
endif()

target_compile_options(RtAudio
    PRIVATE
        ${NC_COMPILER_FLAGS}
)
