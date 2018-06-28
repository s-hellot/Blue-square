#include <iostream>
#include <cstdlib>
#include <stdio.h>
// GLEW to include before gl.h and glfw3.h
#include <GL/glew.h>

#include <GL/gl.h>
// GLFW : handle window and keyboard
#include <GLFW/glfw3.h>
/* GLM is used for 3D mathematics
#include <glm/glm.hpp>
using namespac glm ;
*/

#include <shader.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm ;

#define WIDTH 1024
#define HEIGHT 768
using namespace std;


GLFWwindow* initGlfwAndWindow () {
    if ( !glfwInit ()) {
        fprintf (stderr, "Failed to initialize GLFW\n") ;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    GLFWwindow* window ;
    window = glfwCreateWindow (WIDTH, HEIGHT, "First Window", NULL, NULL ) ;
    if (window == NULL) {
        fprintf (stderr, "Failed to open GLFW window\n") ;
        glfwTerminate() ;
    }
    glfwMakeContextCurrent(window) ; // current on the calling thread
    return window ;
}

void initGlew () {
    glewExperimental = true ;
    if (glewInit () != GLEW_OK) {
        fprintf (stderr, "Failed to initialize GLEW\n") ;
    }
}

bool closeWindow (GLFWwindow* window) {
    return ((glfwGetKey (window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) || (glfwWindowShouldClose(window) != 0) || (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)) ;
}
int main()
{
    GLFWwindow* window = initGlfwAndWindow() ;
    initGlew() ;
    // STICKY KEYS : if you press a key and release it, GFLW_PRESS will be true even after release
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    glClearColor(0,255,0,0) ;
    GLuint programID = LoadShaders("ProjecVertexShader.vertexshader", "ProjecFragmentShader.fragmentshader") ;
    float vertex [] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};
    static const GLfloat g_color_buffer_data[] = {
 0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};
    mat4 projection, view, model, model_tri ;
    model = mat4(1.0f) ; // Id4 because the center of the model is the center of the screen
    view = lookAt(vec3(4,-3,-3), vec3(0,0,0), vec3(0,1,0)) ;
    projection = glm::perspective(radians (45.0f), 4.0f/3.0f, 0.10f, 100.0f) ;
    mat4 MVPmatrix = projection * view * model ;
    GLuint matrixID = glGetUniformLocation  (programID, "MVP") ; //tell GLSL the location of MVP
    GLuint vaoID ;
    GLuint *vboID = (GLuint*) malloc (sizeof (GLuint)*2);
     //create VAO which contains every information about the location and state of the VBO in VRAM
    glGenVertexArrays(1,&vaoID) ;
    glBindVertexArray (vaoID) ;
    // create VBO which allocate space in the VRAM
    glGenBuffers (2, vboID) ;
    // unlock it
    glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
    // fill the space in VRAM with the vertex
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof (g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW) ;
    glEnable (GL_DEPTH_TEST) ;
    do {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ; // reset setting and screen to set previously
        glUseProgram (programID) ; // use the shader
        glUniformMatrix4fv (matrixID, 1, GL_FALSE, &MVPmatrix[0][0]) ; // the value of MVPmatrix to GLSL
        glEnableVertexAttribArray (1) ;
        glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
        glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ;
        glEnableVertexAttribArray (0) ; // tells which VAO stores the data we want to draw ?
        glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ; // unlock buffer twice ?
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0) ; // tells how the data should be read
        glDrawArrays(GL_TRIANGLES, 0 , 12*3) ; //render the data
        glDisableVertexAttribArray (0) ;
        glDisableVertexAttribArray (1) ;
        glfwSwapBuffers(window) ;
        glfwPollEvents() ; // process events already in the event queue
        //getKey uses qwerty keyboard
    } while (closeWindow(window) == 0)  ;
    glDeleteVertexArrays (1, &vaoID) ;
    glDeleteBuffers (1, vboID) ;
    glDeleteProgram (programID) ;
    return 0;
}
