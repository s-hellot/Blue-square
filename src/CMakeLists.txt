#cmake_minimum_required (VERSION 2.6)

#project (Hello_World)


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#Encontrando y linkeando glfw3
#find_package(GLFW REQUIRED)

##include_directories(${GLFW_INCLUDE_DIRS})
#link_directories(${GLFW_LIBRARY_DIRS})
#add_definitions(${GLFW_DEFINITIONS})
include_directories(C:/Users/Simon/Documents/glfw-3.2.1.bin.WIN32/glfw-3.2.1.bin.WIN32/include)
link_directories(C:/Users/Simon/Documents/glfw-3.2.1.bin.WIN32/glfw-3.2.1.bin.WIN32/lib-mingw)

include_directories(C:/Users/Simon/Documents/glew-2.1.0/include/)
link_directories(C:/Users/Simon/Documents/glew-2.1.0/bin/lib)

#add_definitions(-DSHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(../include)
include_directories(../contrib)
include_directories(../textures)

add_library(SmallGL shader.cpp ../include/shader.hpp)
target_link_libraries(SmallGL ${OPENGL_LIBRARIES} glew32) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})

SET (SHADER_FILES
    ../shader/SimpleFragmentShader.fragmentshader ../shader/SimpleVertexShader.vertexshader ../shader/ProjecFragmentShader.fragmentshader ../shader/ProjecVertexShader.vertexshader ../shader/TextFragmentShader.fragmentshader ../shader/TextVertexShader.vertexshader
)
file(COPY ${SHADER_FILES} DESTINATION  ${CMAKE_BINARY_DIR}/src)

add_executable(HelloOpenGL hello_opengl.cpp ../shader/SimpleFragmentShader.fragmentshader ../shader/SimpleVertexShader.vertexshader)
add_executable(Projection projection.cpp ../shader/ProjecVertexShader.vertexshader ../shader/ProjecFragmentShader.fragmentshader Viewport.cpp ../include/Viewport.hpp)
add_executable(Texture texture.cpp ../shader/TextFragmentShader.fragmentshader ../shader/TextVertexShader.vertexshader Viewport.cpp ../include/Viewport.hpp)

target_link_libraries(HelloOpenGL ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
target_link_libraries(Projection ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
target_link_libraries(Texture ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
