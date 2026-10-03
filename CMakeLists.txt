cmake_minimum_required(VERSION 3.15)
project(audy C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add executable target. WIN32 specifies it's a Windows GUI application (avoids console window)
add_executable(audy WIN32
    src/main.c
    src/audio_output.c
    src/resource.rc
)

# Specify include directories
target_include_directories(audy PRIVATE src)

# Link required Windows libraries
target_link_libraries(audy PRIVATE
    comctl32
    ole32
    shcore
    winmm
)

# Add source character set option for MSVC (equivalent to /source-charset:utf-8)
if(MSVC)
    target_compile_options(audy PRIVATE "/source-charset:utf-8")
endif()
