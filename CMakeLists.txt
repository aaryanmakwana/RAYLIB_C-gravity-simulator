cmake_minimum_required(VERSION 3.15)
project(gravity_sim_raylib C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find raylib
find_package(raylib 5.0 QUIET)

if (NOT raylib_FOUND)
    # If raylib is not found, fetch it from GitHub
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Source files
set(SOURCES
    main.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform-specific settings
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl rt)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm gdi32)
endif()

# Set binary name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gravity_sim")

# Installation rules
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

