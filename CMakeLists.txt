cmake_minimum_required(VERSION 3.14)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 17)

set(SOFTRENDERER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ==============================================================================
# Headers and sources
# ==============================================================================
set(SOFTRENDERER_HEADERS
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Maths.h
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/TGAImage.h
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Model.h
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Graphics.h
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Texture.h
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Camera.h
    "${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Platform/Win32.h"
    
)

set(SOFTRENDERER_SOURCES
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/main.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Maths.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/TGAImage.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Model.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Graphics.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Texture.cpp
    ${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Core/Camera.cpp
    "${SOFTRENDERER_ROOT_DIR}/SoftRenderer/Platform/Win32.cpp"

)

# ==============================================================================
# Target definition
# ==============================================================================

set(TARGET SoftRenderer)

add_executable(${TARGET} ${SOFTRENDERER_HEADERS} ${SOFTRENDERER_SOURCES})

# ==============================================================================
# Compile options
# ==============================================================================

if(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /D_CRT_SECURE_NO_WARNINGS)
    target_compile_options(${TARGET} PRIVATE /fp:fast)
else()
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(${TARGET} PRIVATE -ffast-math)
endif()

if(UNIX AND NOT APPLE)
    target_compile_options(${TARGET} PRIVATE -D_POSIX_C_SOURCE=200809L)
endif()

# ==============================================================================
# Link libraries
# ==============================================================================

if(WIN32)
    # nothing to do for now
elseif(APPLE)
    target_link_libraries(${TARGET} PRIVATE "-framework Cocoa")
else()
    target_link_libraries(${TARGET} PRIVATE m X11)
endif()

# ==============================================================================
# Asset files
# ==============================================================================

# set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/Assets")
# set(ASSETS_DST "$<TARGET_FILE_DIR:${TARGET}>/Assets")

# if(WIN32)
#     set(ASSETS_CMD copy_directory)
#     set(ASSETS_MSG "Copy assets")
# else()
#     set(ASSETS_CMD create_symlink)
#     set(ASSETS_MSG "Link assets")
# endif()

# add_custom_command(
#     TARGET ${TARGET} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E echo
#     "${ASSETS_MSG}: ${ASSETS_SRC} to ${ASSETS_DST}"
# )
# add_custom_command(
#     TARGET ${TARGET} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E ${ASSETS_CMD}
#     "${ASSETS_SRC}" "${ASSETS_DST}"
# )


# ==============================================================================
# IDE support
# ==============================================================================

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${TARGET})
source_group(TREE "${SOFTRENDERER_ROOT_DIR}/SoftRenderer" FILES ${SOFTRENDERER_HEADERS} ${SOFTRENDERER_SOURCES})