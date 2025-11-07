# Falling Fury - Modern CMake Configuration
cmake_minimum_required(VERSION 3.25)
project(FallingFury VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Header files
file(GLOB_RECURSE HEADER_FILES
     "${CMAKE_SOURCE_DIR}/include/*.h"
)

# Source files
file(GLOB_RECURSE SOURCE_FILES
     "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# Find SFML
if(APPLE)
    # macOS: Use frameworks from local SFML directory
    set(SFML_ROOT "${CMAKE_SOURCE_DIR}/SFML")
    set(CMAKE_FRAMEWORK_PATH "${SFML_ROOT}/Frameworks;${SFML_ROOT}/extlibs")
    
    find_library(SFML_GRAPHICS sfml-graphics PATHS "${SFML_ROOT}/Frameworks" NO_DEFAULT_PATH)
    find_library(SFML_WINDOW sfml-window PATHS "${SFML_ROOT}/Frameworks" NO_DEFAULT_PATH)
    find_library(SFML_SYSTEM sfml-system PATHS "${SFML_ROOT}/Frameworks" NO_DEFAULT_PATH)
    find_library(SFML_AUDIO sfml-audio PATHS "${SFML_ROOT}/Frameworks" NO_DEFAULT_PATH)
    
    # Find extlibs (dependencies)
    find_library(FREETYPE_LIB freetype PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(OGG_LIB ogg PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(VORBIS_LIB vorbis PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(VORBISENC_LIB vorbisenc PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(VORBISFILE_LIB vorbisfile PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(FLAC_LIB FLAC PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    find_library(OPENAL_LIB OpenAL PATHS "${SFML_ROOT}/extlibs" NO_DEFAULT_PATH)
    
    set(SFML_LIBRARIES ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM} ${SFML_AUDIO})
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    
    message(STATUS "Using local SFML frameworks from: ${SFML_ROOT}")
else()
    # Linux/Windows: Use find_package
    find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
    set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system sfml-audio)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${SFML_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        ${SFML_LIBRARIES}
)

# macOS: Set rpath to find frameworks at runtime
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/Frameworks;@loader_path/Frameworks"
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
    
    # Copy SFML frameworks and extlibs to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SFML_ROOT}/Frameworks ${CMAKE_BINARY_DIR}/bin/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SFML_ROOT}/extlibs ${CMAKE_BINARY_DIR}/bin/Frameworks
        COMMENT "Copying SFML frameworks and dependencies to build directory"
    )
endif()

# Compile definitions
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
        $<$<CONFIG:Release>:RELEASE_MODE>
)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/)
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR}/bin/)

# Create data directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/data)

# Install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)
install(DIRECTORY data DESTINATION bin)

# Print configuration summary
message(STATUS "")
message(STATUS "====================================")
message(STATUS "Falling Fury - Configuration Summary")
message(STATUS "====================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "====================================")
message(STATUS "")