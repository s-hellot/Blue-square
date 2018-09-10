#cmake_minimum_required (VERSION 2.6)

#project (Hello_World)


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories(C:/Users/Simon/Documents/glfw-3.2.1.bin.WIN32/glfw-3.2.1.bin.WIN32/include)
link_directories(C:/Users/Simon/Documents/glfw-3.2.1.bin.WIN32/glfw-3.2.1.bin.WIN32/lib-mingw)

include_directories(C:/Users/Simon/Documents/glew-2.1.0/include/)
link_directories(C:/Users/Simon/Documents/glew-2.1.0/bin/lib)

include_directories(../contrib)

set(CMAKE_MODULE_PATH "C:/Users/Simon/Documents/VisualizationLibrary/VL_install/cmake")
#set(CMAKE_MODULE_PATH ${VL_ROOT}/cmake)
#set(VL_ROOT "C:/Users/Simon/Documents/VisualizationLibrary/VL_install")
# VL_ROOT to be defined as the VL install directory

find_package(VL COMPONENTS VLCore VLGraphics VLVolume VLWin32 VLGLFW VLMain REQUIRED)
IF (VL_FOUND)
    include_directories(${VL_INCLUDE_DIRS})

    add_executable(RotatingCube  ../contrib/VL/App_VolumeRaycast.cpp ../contrib/VL/BaseDemo.hpp ../contrib/VL/GLFW_example.cpp ../contrib/VL/App_RotatingCube.hpp )
    target_link_libraries(RotatingCube ${VL_LIBRARIES} ${OPENGL_LIBRARIES} glfw3)

    #message(FATAL_ERROR '${VL_LIBRARIES}')

    SET (SHADER_FILES
        ${SHADER_FILES}
    )
    #add_executable(App_VolumeRaycast ${SHADER_FILES}  ../contrib/VL/Win32_example_volume.cpp ../contrib/VL/BaseDemo.hpp ../contrib/VL/App_VolumeRaycast.cpp )
    #target_link_libraries(App_VolumeRaycast ${VL_LIBRARIES} ${OPENGL_LIBRARIES})



ENDIF (VL_FOUND)

#Encontrando y linkeando glfw3
#find_package(GLFW REQUIRED)

##include_directories(${GLFW_INCLUDE_DIRS})
#link_directories(${GLFW_LIBRARY_DIRS})
#add_definitions(${GLFW_DEFINITIONS})



#add_definitions(-DSHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(../include)
include_directories(../textures)

add_library(SmallGL shader.cpp ../include/shader.hpp)
target_link_libraries(SmallGL ${OPENGL_LIBRARIES} glew32) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})

SET (SHADER_FILES
    ${SHADER_FILES}
    ../shader/SimpleFragmentShader.fragmentshader ../shader/SimpleVertexShader.vertexshader
    ../shader/ProjecFragmentShader.fragmentshader ../shader/ProjecVertexShader.vertexshader
    ../shader/TextFragmentShader.fragmentshader ../shader/TextVertexShader.vertexshader .
    ../shader/TextureLine.vertexshader ../shader/TextureLine.fragmentshader
    ../shader/Pea.vertexshader ../shader/Pea.fragmentshader
)

SET (CXX Viewport.cpp Exception.cpp OpenGLException.cpp Image.cxx Rectangle.cpp Pea.cpp )
SET (HXX ../include/Viewport.hpp ../include/Exception.hpp ../include/OpenGLException.hpp ../include/Image.inl ../include/Image.h ../include/Rectangle.hpp ../include/Pea.hpp )
file(COPY ${SHADER_FILES} DESTINATION  ${CMAKE_BINARY_DIR}/src)

add_executable(HelloOpenGL hello_opengl.cpp ../shader/SimpleFragmentShader.fragmentshader ../shader/SimpleVertexShader.vertexshader)
add_executable(Projection projection.cpp ${CXX} ${HXX} ../shader/ProjecVertexShader.vertexshader ../shader/ProjecFragmentShader.fragmentshader)
add_executable(Texture texture.cpp ${HXX} ${CXX} ../shader/TextFragmentShader.fragmentshader ../shader/TextVertexShader.vertexshader
                ../shader/TextureLine.vertexshader ../shader/TextureLine.fragmentshader ../shader/Pea.vertexshader ../shader/Pea.fragmentshader
                )

target_link_libraries(HelloOpenGL ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
target_link_libraries(Projection ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
target_link_libraries(Texture ${OPENGL_LIBRARIES} glfw3 glew32 SmallGL) #${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLM_LIBRARIES})
