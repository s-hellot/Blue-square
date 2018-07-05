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
using namespace glm ;

#define WIDTH 1024
#define HEIGHT 768
using namespace std;

vec3 cameraPos = vec3 (0.0f, 0.0f, 6.0f) ;
vec3 cameraFront = vec3 (0.0f, 0.0f, -1.0f) ;
vec3 cameraUp = vec3 (0.0f, 1.0f, 0.0f) ;
float deltaTime = 0.0f ;
bool firstMouse = false ;
// true if it isn't the first time the mouse appears on the screen
float lastX, lastY ;
float pitch = 0, yaw = -90 ;
// yaw is init at -90 to remove the jump in x at first mouse input (still exist in z-axis)
float fov = 45 ;

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

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods){
    float cameraSpeed = 15.0f * deltaTime ;
    switch (key) {
    case GLFW_KEY_W :
        cameraPos += cameraSpeed * cameraFront ;
        break ;
    case GLFW_KEY_S :
        cameraPos -= cameraSpeed * cameraFront ;
        break ;
    case GLFW_KEY_A :
        cameraPos -= normalize(cross (cameraFront, cameraUp))* cameraSpeed ;
        break ;
    case GLFW_KEY_D :
        cameraPos += normalize(cross (cameraFront, cameraUp))* cameraSpeed ;
        break ;
    }
}

void mouseCallback (GLFWwindow* window, double xpos, double ypos) {
    float xoffset, yoffset ;
    if (firstMouse) {
        lastX = xpos ;
        lastY = ypos ;
    } else {
        xoffset = xpos - lastX ;
        yoffset = lastY - ypos ;
        // reversed with Y because y axis is bottom to top on the screen coordinate
        // but we want to look up when the mouse cursor goes up (camera y-axis is opposite)
        lastX = xpos ;
        lastY = ypos ;
        float sensitivity = 0.05f ;
        xoffset *= sensitivity ;
        yoffset *= sensitivity ;

        yaw += xoffset ;
        pitch += yoffset ;

        if (pitch > 89) {
            pitch = 89 ;
        } else if (pitch < - 89) {
            pitch = -89 ;
        }
        // you can only see above and below you not behind you
        vec3 frontDirection ;
        frontDirection.x = cos (radians(pitch)) * cos(radians(yaw)) ;
        frontDirection.y = sin(radians(pitch)) ;
        frontDirection.z = cos(radians(pitch)) * sin(radians(yaw)) ;
        cameraFront = normalize(frontDirection) ;
    }
}

void scroll_callback (GLFWwindow* window, double xoffset, double yoffset) {
    fov += yoffset ;
    fov = (fov > 45)? 45 : fov ;
    fov = (fov <  1)? 1  : fov ;
}
int main()
{
    GLFWwindow* window = initGlfwAndWindow() ;
    initGlew() ;
    // STICKY KEYS : if you press a key and release it, GFLW_PRESS will be true even after release
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glClearColor(0,255,0,0) ;
    GLuint programID = LoadShaders("ProjecVertexShader.vertexshader", "ProjecFragmentShader.fragmentshader") ;
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    } ;
    vec3 cubePositions[] = {
  vec3( 0.0f,  0.0f,  0.0f),
  vec3( 2.0f,  5.0f, -15.0f),
  vec3(-1.5f, -2.2f, -2.5f),
  vec3(-3.8f, -2.0f, -12.3f),
  vec3( 2.4f, -0.4f, -3.5f),
  vec3(-1.7f,  3.0f, -7.5f),
  vec3( 1.3f, -2.0f, -2.5f),
  vec3( 1.5f,  2.0f, -2.5f),
  vec3( 1.5f,  0.2f, -1.5f),
  vec3(-1.3f,  1.0f, -1.5f)
};
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    mat4 projection, view, model ;
    //projection = perspective(radians (45.0f), 1024.0f/768.0f, 0.10f, 100.0f) ;
    //projection = ortho (4.0f, -4.0f, -5.0f, 5.0f, 0.1f, 100.0f) ;
    GLuint projID = glGetUniformLocation  (programID, "projection") ; //tell GLSL the location of MVP
    GLuint viewID = glGetUniformLocation  (programID, "view") ;
    GLuint modelID = glGetUniformLocation  (programID, "model") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
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
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) ;
    glEnableVertexAttribArray(0) ;
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) 0 ) ;
    glEnableVertexAttribArray (1) ;
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (3*sizeof(float))) ;
    GLuint textureID = loadTexture() ;
    GLuint textureSampler = glGetUniformLocation (programID, "textSampler") ;
    GLFWkeyfun keyFunc = &keyboard ;
    glfwSetKeyCallback(window, keyFunc) ;
    GLFWcursorposfun cursorFunc = mouseCallback ;
    glfwSetCursorPosCallback(window, cursorFunc) ;
    GLFWscrollfun scrollFunc = scroll_callback ;
    glfwSetScrollCallback(window, scrollFunc) ;
    glEnable (GL_DEPTH_TEST) ;
    float lastFrame = 0.0f ;
    do {
        float currentFrame = glfwGetTime() ;
        deltaTime = currentFrame - lastFrame ;
        lastFrame = currentFrame ;
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ; // reset setting and screen to set previously
        glUseProgram (programID) ; // use the shader
        float camX, camZ ;
        //camX= sin(glfwGetTime())*10 ;
        //camZ= cos(glfwGetTime())*10 ;
        view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp) ;
        projection = perspective(radians (fov), 1024.0f/768.0f, 0.10f, 100.0f) ;
        glUniformMatrix4fv (projID, 1, GL_FALSE, &projection[0][0]) ;
        glUniformMatrix4fv (viewID, 1, GL_FALSE, &view[0][0]) ;
        glActiveTexture(GL_TEXTURE0) ;
        glBindTexture(GL_TEXTURE_2D, textureID) ;
        glUniform1i (textureSampler, 0) ;
        for (int i = 0 ; i < 10 ; i++) {
                model = translate(mat4(1.0f),cubePositions[i]) ;
                //float angle = glfwGetTime()*20 ;
                //model = rotate(model, angle, vec3(1,1,1)) ;
                glUniformMatrix4fv (modelID, 1, GL_FALSE, &model[0][0]) ;
                glDrawArrays(GL_TRIANGLES, 0 , 12*3) ; //render the data
        }
        glfwSwapBuffers(window) ;
        glfwPollEvents() ; // process events already in the event queue
        //getKey uses qwerty keyboard
    } while (closeWindow(window) == 0)  ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1) ;
    glDeleteVertexArrays (1, &vaoID) ;
    glDeleteBuffers (1, &vboID) ;
    glDeleteProgram (programID) ;
    return 0;
}
