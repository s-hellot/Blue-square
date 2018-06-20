cmake_minimum_required (VERSION 2.6)

project (Hello_World)


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

include_directories(C:/Users/Simon/Projets/MV/Shader)
add_executable(HelloOpenGL hello_opengl.cpp Shader/shader.cpp Shader/shader.hpp SimpleFragmentShader.fragmentshader SimpleVertexShader.vertexshader)
target_link_libraries(HelloOpenGL ${OPENGL_LIBRARIES} glfw3 glew32) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
