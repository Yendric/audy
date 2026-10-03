cmake_minimum_required(VERSION 3.15)
project(audy VERSION 1.1 LANGUAGES C RC)

add_executable(audy WIN32 src/main.c src/audio_output.c src/resource.rc)

set_target_properties(audy PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)

target_compile_definitions(audy PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN
    VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${PROJECT_VERSION_MINOR}
)

target_link_libraries(audy PRIVATE comctl32 ole32)

if(MSVC)
    target_compile_options(audy PRIVATE $<$<COMPILE_LANGUAGE:C>:/W4 /utf-8>)
    # The manifest is embedded through resource.rc
    target_link_options(audy PRIVATE /MANIFEST:NO)
else()
    target_compile_options(audy PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra>)
    target_link_options(audy PRIVATE -municode)
endif()
