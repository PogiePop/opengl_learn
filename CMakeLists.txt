cmake_minimum_required(VERSION 3.15)

project(learnopengl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
endif()

#若没有glm,自行安装
set(GLM_SRC C:\\glm-0.9.8.5\\glm)
set(GLFW_SRC C:\\glfw-3.4.bin.WIN64\\glfw-3.4.bin.WIN64\\)
set(GLFW_INCLUDE ${GLFW_SRC}/include)
set(GLFW_LIB ${GLFW_SRC}/lib-vc2022)
set(GLAD_ROOT C:\\glad)
set(IMGUI_SRC C:\\IMGUI)
#assimp库
set(ASSIMP_SRC C:\\assimp\\6.0.2\\assimp)
set(ASSIMP_DLL ${ASSIMP_SRC}/build/bin/Debug/assimp-vc143-mtd.dll)
set(ASSIMP_LIB ${ASSIMP_SRC}/build/lib/Debug)
set(ASSIMP_CONFIG ${ASSIMP_SRC}/build/include)
set(ASSIMP_INCLUDE ${ASSIMP_SRC}/include)

link_directories(${GLFW_LIB})
link_directories(${ASSIMP_LIB})
include_directories(${GLFW_INCLUDE})
include_directories(${GLM_SRC})
include_directories(${GLAD_ROOT}/include)
#添加tools
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools)

include_directories(${IMGUI_SRC})
include_directories(${ASSIMP_CONFIG})
include_directories(${ASSIMP_INCLUDE})


#将glad链接进COMMON_LIBS
add_library(glad STATIC
    ${GLAD_ROOT}/src/glad.c
)

#将stbi_image链接进COMMON_LIBS
add_library(stbi STATIC
    tools/stb_image.cpp
)

#添加imgui
add_library(imgui
    STATIC
    ${IMGUI_SRC}/imgui.cpp
    ${IMGUI_SRC}/imgui_demo.cpp
    ${IMGUI_SRC}/imgui_draw.cpp
    ${IMGUI_SRC}/backends/imgui_impl_glfw.cpp
    ${IMGUI_SRC}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_SRC}/imgui_tables.cpp
    ${IMGUI_SRC}/imgui_widgets.cpp
)

#添加glfw.lib,以及windows上必须的opengl32.lib
set(COMMON_LIBS glfw3 opengl32 glad stbi imgui assimp-vc143-mtd) 

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)


