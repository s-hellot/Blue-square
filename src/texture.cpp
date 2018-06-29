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

#include "cloud.h"
#include "LUT.h"
using namespace glm ;

//#include "checker.c"
#define WIDTH 1024
#define HEIGHT 768
using namespace std;

// lut.h data in a big 1D array
// change when click
// try other LUT table ?

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

bool checkGLError(string FILE, string FUNCTION, int LINE) {
    GLenum err = glGetError() ;
    switch (err) {
        case GL_INVALID_ENUM :
            cerr << "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag. In "  << FILE << " in " << FUNCTION << " at " << LINE << endl ;
            break ;
        case GL_INVALID_VALUE :
            cerr << "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. In "  << FILE << " in " << FUNCTION << " at " << LINE << endl ;
            break ;
        case GL_INVALID_OPERATION :
            cerr << "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. In "  << FILE << " in " << FUNCTION << " at " << LINE << endl ;
            break ;
        case GL_INVALID_FRAMEBUFFER_OPERATION :
            cerr << "The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE).  In "  << FILE << " in " << FUNCTION << " at " << LINE << endl ;
            break ;
        case GL_OUT_OF_MEMORY :
            cerr << "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. In "  << FILE << " in " << FUNCTION << " at " << LINE << endl ;
            break ;
    }
    return (err !=  GL_NO_ERROR) ;
}
GLuint loadTexture () {
    char* p_data = new char[g_cloud_texture_width * g_cloud_texture_height];
    if (!p_data)
    {
        cerr << "Not enough memory" << endl;
        exit(EXIT_FAILURE);
    }

    char* p_temp = g_cloud_texture_data;
    unsigned char p_pixel[3];
    for (int i = 0; i < g_cloud_texture_width * g_cloud_texture_height; ++i)
    {
        CLOUD_HEADER_PIXEL(p_temp, p_pixel);
        p_data[i] = p_temp[0];
    }

    GLuint textureID ;
    glGenTextures (1, &textureID) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindTexture (GL_TEXTURE_2D, textureID) ;
    float pixels [] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
    } ;
/*    float lum [4] ;
    for (int i = 0 ; i < 4 ; i++) {
        lum[i] = 0.2126*pixels[i*3] + 0.7152*pixels[(i+1)*3] + 0.0722*pixels[(i+2)*3] ;
    }
 */
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    //glTexImage2D(GL_PROXY_TEXTURE_2D,0,GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels) ; // check if their is enough memory in the GPU if not set image state to 0
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RED, 2, 2, 0, GL_RED, GL_FLOAT, pixels) ;
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED, g_cloud_texture_width, g_cloud_texture_height, 0, GL_RED, GL_BYTE, p_data) ; // load the image
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // interpolation mode

    delete [] p_data;
    return textureID ;
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
GLuint loadLUT () {
    unsigned char *pixels = new unsigned char [g_lut_texture_width*3] ;
    for (int i =  0 ; i < g_lut_texture_width ; ++i) {
        pixels[3 * i    ] = g_lut_texture_data[i][0] ;
        pixels[3 * i + 1] = g_lut_texture_data[i][1] ;
        pixels[3 * i + 2] = g_lut_texture_data[i][2] ;
    }
    for (int i = 0 ; i < 3*g_lut_texture_width - 2 ; i+=3) {
        cout << (int) pixels [i] << ";" << (int) pixels [i+1] << ";" << (int) pixels [i+2] << endl ;
    }
    GLuint textureID ;
    glGenTextures (1, &textureID) ;
    glBindTexture (GL_TEXTURE_1D, textureID) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, g_lut_texture_width, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels) ; // load the image
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //wrapping mode
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // interpolation mode
    return textureID ;
}

int main()
{
    GLFWwindow* window = initGlfwAndWindow() ;
    initGlew() ;
    // STICKY KEYS : if you press a key and release it, GFLW_PRESS will be true even after release
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    glClearColor(0,255,0,0) ;
    GLuint programID = LoadShaders("TextVertexShader.vertexshader", "TextFragmentShader.fragmentshader") ;
    if (programID == 0 ) {
            return 0 ;
    } else {
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        float vertex [] = { -0.5, -0.5, 0,
                            -0.5,  0.5, 0,
                             0.5, -0.5, 0,
                            -0.5,  0.5, 0,
                             0.5, -0.5, 0,
                             0.5,  0.5, 0 } ;
        float textureCoord [] = {
                             0.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f
        } ;
        GLuint vaoID ;
        GLuint *vboID =  (GLuint*) malloc (2*sizeof(GLuint));
         //create VAO which contains every information about the location and state of the VBO in VRAM
        glGenVertexArrays(1,&vaoID) ;
        glBindVertexArray (vaoID) ;
        // create VBO which allocate space in the VRAM
        glGenBuffers (2, vboID) ;
        // unlock it
        glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
        // fill the space in VRAM with the vertex
        glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
        glBufferData (GL_ARRAY_BUFFER, sizeof(textureCoord), textureCoord, GL_STREAM_DRAW) ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        GLuint lutID = loadLUT() ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        GLuint textureID = loadTexture() ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        GLuint textureSampler = glGetUniformLocation (programID, "myTextureSampler") ; //allocate the memory for uniform variable myTextureSample
        GLuint brightness = glGetUniformLocation (programID, "bright") ;
        GLuint contrast = glGetUniformLocation(programID, "contr") ;
        GLuint lutSampler = glGetUniformLocation (programID, "myLutSampler") ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
        double xpos, ypos ;
        do {
            glClear ( GL_COLOR_BUFFER_BIT) ; // reset setting and screen to set previously
            glUseProgram (programID) ; // use the shader
            glfwGetCursorPos(window, &xpos, &ypos) ;
            glActiveTexture(GL_TEXTURE0) ;
            glBindTexture(GL_TEXTURE_2D, textureID) ;
            glUniform1i (textureSampler, 0) ;
            glActiveTexture(GL_TEXTURE1) ;
            glBindTexture(GL_TEXTURE_1D, lutID) ;
            glUniform1i (lutSampler, 0) ;
            // brightness 0 at the middle -1 at left corner and 1 at right corner
            glUniform1f (brightness, (float) (ypos-384)/384) ;
            // contrast 0 at the left corner 1 at the middle and 2 at the right corner
            glUniform1f (contrast, (float) xpos/612) ;
            glEnableVertexAttribArray (0) ; // tells which VAO stores the data we want to draw ?
            glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
            glEnableVertexAttribArray (1) ;
            glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
            glDrawArrays(GL_TRIANGLES, 0 , 6) ; //render the data
            glDisableVertexAttribArray (0) ;
            glDisableVertexAttribArray (1) ;
            glfwSwapBuffers(window) ;
            glfwPollEvents() ; // process events already in the event queue
            //checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
            //getKey uses qwerty keyboard
        } while (closeWindow(window) == 0)  ;
        glDeleteVertexArrays (1, &vaoID) ;
        glDeleteBuffers (2, vboID) ;
        glDeleteProgram (programID) ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
       // glDeleteTextures(1, &textureID) ;
    }
    return 0;
}

