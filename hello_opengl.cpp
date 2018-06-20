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

#define WIDTH 1024
#define HEIGHT 768
using namespace std;

int main()
{
    // Initialise GLFW
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
        return -1 ;
    }
    glfwMakeContextCurrent(window) ; // current on the calling thread
    glewExperimental = true ;
    // Initialize GLEW
    if (glewInit () != GLEW_OK) {
        fprintf (stderr, "Failed to initialize GLEW\n") ;
        return -1 ;
    }
    // STICKY KEYS : if you press a key and release it, GFLW_PRESS will be true even after release
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    glClearColor(0,255,0,0) ;
    float vertex [] = { -0.5, -0.5, 0,
                    -0.5, 0.5, 0,
                    0.5, -0.5, 0,
                    0.5, -0.5, 0,
                    -0.5, 0.5, 0,
                    0.5, 0.5, 0 } ;
    GLuint vaoID ;
    GLuint vboID ;
     //create VAO which contains every information about the location and state of the VBO in VRAM
    glGenVertexArrays(1,&vaoID) ;
    glBindVertexArray (vaoID) ;
    // create VBO which allocate space in the VRAM
    glGenBuffers (1, &vboID) ;
    // unlock it
    glBindBuffer (GL_ARRAY_BUFFER, vboID) ;
    // fill the space in VRAM with the vertex
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;
    do {
        glClear ( GL_COLOR_BUFFER_BIT) ; // reset setting and screen to set previously
        glEnableVertexAttribArray (0) ; // tells which VAO stores the data we want to draw ?
        glBindBuffer (GL_ARRAY_BUFFER, vboID) ; // unlock buffer twice ?
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
        glDrawArrays(GL_TRIANGLES, 0 , 6) ; //render the data
        glDisableVertexAttribArray (0) ;
        glfwSwapBuffers(window) ;
        glfwPollEvents() ; // process events already in the event queue
        //getKey uses qwerty keyboard
    } while ((glfwGetKey (window, GLFW_KEY_ESCAPE ) != GLFW_PRESS) && (glfwWindowShouldClose(window) == 0) && (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS))  ;
    return 0;
}
