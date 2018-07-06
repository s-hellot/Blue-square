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
#include "LUT_CYAN.h"
#include "LUT_FIRE.h"
#include "LUT_GFB.h"
#include "LUT_ICE.h"

using namespace glm ;

//#include "checker.c"
#define WIDTH_INIT 800.0
#define HEIGHT_INIT 600.0
#define RATIO_INIT (WIDTH_INIT/HEIGHT_INIT)
using namespace std;

// cmake rule to copy only modified shader file

void windowSize (GLFWwindow* window, int width, int height) ;
GLFWwindow* initGlfwAndWindow () {
    if ( !glfwInit ()) {
        cerr << "Failed to initialize GLFW\n" << endl ;
        exit(EXIT_FAILURE) ;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE) ;

    GLFWwindow* window ;
    window = glfwCreateWindow (WIDTH_INIT, HEIGHT_INIT, "First Window", NULL, NULL ) ;
    if (window == NULL) {
        cerr << "Failed to open GLFW window \n" << endl ;
        glfwTerminate() ;
        exit (EXIT_FAILURE) ;
    }
    glfwMakeContextCurrent(window) ; // current on the calling thread
    windowSize(window, WIDTH_INIT, HEIGHT_INIT) ;
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
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexImage2D(GL_PROXY_TEXTURE_2D,0,GL_RED, g_cloud_texture_width, g_cloud_texture_height, 0, GL_RED, GL_BYTE, p_data) ; // load the image
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
GLuint loadLUT (unsigned char data [256][3]) {
    unsigned char *pixels = new unsigned char [256*3] ;
    for (int i =  0 ; i < 256 ; ++i) {
        pixels[3 * i    ] = data[i][0] ;
        pixels[3 * i + 1] = data[i][1] ;
        pixels[3 * i + 2] = data[i][2] ;
    }
    GLuint textureID ;
    glGenTextures (1, &textureID) ;
    glBindTexture (GL_TEXTURE_1D, textureID) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels) ; // load the image
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //wrapping mode
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // interpolation mode
    return textureID ;
}

// when you click and move the mouse the brightness and contrast change
void cursorMove (GLFWwindow* window, double xpos, double ypos) {
    GLint programID ;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID) ;
    GLuint brightness = glGetUniformLocation (programID, "bright") ;
    GLuint contrast = glGetUniformLocation(programID, "contr") ;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // brightness 0 at the middle -1 at left corner and 1 at right corner
        glUniform1f (brightness, (float) (ypos-HEIGHT_INIT/2)/(HEIGHT_INIT/2)) ;
        // contrast 0 at the left corner 1 at the middle and 2 at the right corner
        glUniform1f (contrast, (float) xpos/(WIDTH_INIT/2)) ;
    }
}

/* when you right click the picture is reset to no brightness and contrast change
 when you left click you change brightness and contrast (if you just left click without moving the mouse
                                                        it isn't treated in cursorMove)*/
void mouseButton (GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos ;
    GLint programID ;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID) ;
    GLuint brightness = glGetUniformLocation (programID, "bright") ;
    GLuint contrast = glGetUniformLocation(programID, "contr") ;
    if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS)) {
        // no change at the picture => brightness = 0 contrast = 1
        glUniform1f (brightness, 0.0f) ;
        glUniform1f (contrast, 1.0f) ;
    } else if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        glfwGetCursorPos(window, &xpos, &ypos) ;
        glUniform1f (brightness, (float) ((ypos-HEIGHT_INIT/2)/HEIGHT_INIT/2)) ;
        // brightness 0 at the middle -1 at left corner and 1 at right corner
        glUniform1f (contrast, xpos/(WIDTH_INIT/2)) ;
        // contrast 0 at the left corner 1 at the middle and 2 at the right corner
    }
}

void windowSize (GLFWwindow* window, int width, int height) {
    float ratioWindow = ((float) width)/height ;
    if (ratioWindow > RATIO_INIT) {
    // too wide
        width = RATIO_INIT * height ;
    } else if (ratioWindow < RATIO_INIT) {
        height = width / RATIO_INIT  ;
    }
    glViewport (0, 0, width, height) ;
}

GLuint loadShader (char* vertexShader, char* fragmentShader) {
    GLuint programID = LoadShaders(vertexshader, fragmentshader) ;
    if (programID == 0 ) {
            cout << "Error loading shader" << endl ;
            exit(EXIT_FAILURE) ;
    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return programID ;
}


float* loadTextureCoord () {
    float textureCoord [12] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return textureCoord ;
}

GLuint loadAndBindVAO () {
    GLuint vaoID ;
    //create VAO which contains every information about the location and state of the VBO in VRAM
    glGenVertexArrays(1,&vaoID) ;
    glBindVertexArray (vaoID) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return vaoID ;
}

GLuint* loadAndFillVBO (float* vertex, float* textureCoord) {
    GLuint *vboID =  new GLuint[2];
    // create VBO which allocate space in the VRAM
    glGenBuffers (2, vboID) ;
    // unlock it
    glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(textureCoord), textureCoord, GL_STREAM_DRAW) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return vboID ;
}

void generateUniformVariable (GLuint programID, GLuint* textureSampler, GLuint* lutSampler, GLuint* matProj) {
    *textureSampler = glGetUniformLocation (programID, "myTextureSampler") ;
    *lutSampler     = glGetUniformLocation (programID, "myLutSampler") ;
    *matProj        = glGetUniformLocation (programID, "projection") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void setGLFWCallbackFunction (GLFWwindow* window) {
    GLFWcursorposfun callbackCursor = &cursorMove ;
    glfwSetCursorPosCallback(window, callbackCursor) ;
    GLFWmousebuttonfun callbackMouse = &mouseButton ;
    glfwSetMouseButtonCallback(window, callbackMouse) ;
    GLFWwindowsizefun sizeFunc = &windowSize ;
    glfwSetWindowSizeCallback(window, sizeFunc) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void loadSampler (GLuint textureID, GLuint textureSampler, GLuint lutID, GLuint lutSampler) {
    glActiveTexture(GL_TEXTURE0) ;
    glBindTexture(GL_TEXTURE_2D, textureID) ;
    glUniform1i (textureSampler, 0) ;
    glActiveTexture(GL_TEXTURE1) ;
    glBindTexture(GL_TEXTURE_1D, lutID) ;
    glUniform1i (lutSampler, 0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}


void loadDataToShader (GLuint* vboID) {
    glEnableVertexAttribArray (0) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    // send the vertices to the vertex shader
    glEnableVertexAttribArray (1) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void disableData () {
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1)
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void deleteMemory (GLuint programID, GLuint vaoID, GLuint* vboID, GLuint textureID, GLuint lutID) {
    glDeleteVertexArrays (1, &vaoID) ;
    glDeleteBuffers (2, vboID) ;
    glDeleteProgram (programID) ;
    glDeleteTextures(1, &textureID) ;
    glDeleteTextures(1, &lutID) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}
int main()
{
    GLFWwindow* window = initGlfwAndWindow() ;
    initGlew() ;
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    glClearColor(0,255,0,0) ;
    GLuint programID = loadShader("TextVertexShader.vertexshader", "TextFragmentShader.fragmentshader") ;

    float vertex []= { -1, -1, 0,
                      -1,  1, 0,
                       1, -1, 0,
                      -1,  1, 0,
                       1, -1, 0,
                       1,  1, 0 } ;
    float textureCoord [] = {
                             0.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f
    } ;

    GLuint vaoID = loadAndBindVAO();

    GLuint *vboID =  new GLuint [2] ;
    glGenBuffers (2, vboID) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[0]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;
    glBindBuffer (GL_ARRAY_BUFFER, vboID[1]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(textureCoord), textureCoord, GL_STREAM_DRAW) ;
    //load texture data and vertex into the GPU

    GLuint lutID = loadLUT(g_lut_texture_data) ;
    GLuint textureID = loadTexture() ;
    //Create LUT and texture from file

    GLuint textureSampler, lutSampler, matProj ;
    generateUniformVariable(programID, &textureSampler, &lutSampler, &matProj) ;
    //Generate uniform variable location*/

    setGLFWCallbackFunction(window) ;

    mat4 projection = ortho(-1.0f,1.0f,-1.0f,1.0f) ;
    do {
        glClear ( GL_COLOR_BUFFER_BIT) ; // reset setting and screen to set previously
        glUseProgram (programID) ; // use the shader

        loadSampler (textureID, textureSampler, lutID, lutSampler) ;
        glUniformMatrix4fv (matProj, 1, GL_FALSE, &projection[0][0]) ;
        // load projection matrix into the vertex shader

        loadDataToShader (vboID) ;
        glDrawArrays(GL_TRIANGLES, 0 , 6) ; //render the data
        disableData() ;

        glfwSwapBuffers(window) ;
        glfwPollEvents() ;
    } while (closeWindow(window) == 0)  ;
    deleteMemory (programID, vaoID, vboID, textureID, lutID) ;
}


