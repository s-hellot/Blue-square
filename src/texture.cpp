#include <iostream>
#include <cstdlib>
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

#include <Viewport.hpp>
#include <OpenGLException.hpp>

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

float* g_p_brightness, *g_p_contrast ;

void windowSize (GLFWwindow* window, int width, int height) ;
void setGLFWCallbackFunction (GLFWwindow* window) ;
void keepRatio (int* p_width, int* p_height) ;
int getBlock (GLFWwindow* window, int xpos, int ypos) ;
void posBlock (int num_view, int* width, int* height) ;


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

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    setGLFWCallbackFunction(window) ;


    return window ;
}

void initGL () {
    glClearColor(0,255,0,0) ;
}

bool checkGLError(string FILE, string FUNCTION, int LINE) {

    GLenum err = GL_NO_ERROR;

#ifndef NDEBUG
     err = glGetError() ;

    if (err != GL_NO_ERROR) {
        string error_message ;
         switch (err) {
        case GL_INVALID_ENUM :
            error_message = "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag." ;
            break ;
        case GL_INVALID_VALUE :
            error_message = "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. " ;
            break ;
        case GL_INVALID_OPERATION :
            error_message = "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. " ;
            break ;
        case GL_INVALID_FRAMEBUFFER_OPERATION :
            error_message = "The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE). " ;
            break ;
        case GL_OUT_OF_MEMORY :
            error_message = "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. " ;
            break ;
         }
        throw OpenGLException(__FILE__, __FUNCTION__, __LINE__, err, error_message) ;
    }

#endif
    return (err !=  GL_NO_ERROR) ;
}


GLuint loadTexture () {
// load the cloud texture from cloud.h
    char* p_data = new char[g_cloud_texture_width * g_cloud_texture_height];
    if (!p_data)
    {
        cerr << "Not enough memory" << endl;
        exit(EXIT_FAILURE);
    }

    char* p_temp = g_cloud_texture_data;
    unsigned char p_pixel[3];
    #pragma omp parallel for
    for (int i = 0; i < g_cloud_texture_width * g_cloud_texture_height; ++i)
    {
        CLOUD_HEADER_PIXEL(p_temp, p_pixel);
        p_data[i] = p_temp[0];
    }

    GLuint texture_id ;
    glGenTextures (1, &texture_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindTexture (GL_TEXTURE_2D, texture_id) ;
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
    return texture_id ;
}

void initGlew () {
    glewExperimental = true ;
    if (glewInit () != GLEW_OK) {
        cerr << "Failed to initialize GLEW\n" << endl ;
    }
}

bool closeWindow (GLFWwindow* window) {
// check if there is a request to close the window
    return ((glfwGetKey (window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) || (glfwWindowShouldClose(window) != 0) || (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)) ;
}
GLuint loadLUT (unsigned char data [256][3]) {
// load LUT table from header file
    unsigned char *pixels = new unsigned char [256*3] ;
    #pragma omp parallel for
    for (int i =  0 ; i < 256 ; ++i) {
        pixels[3 * i    ] = data[i][0] ;
        pixels[3 * i + 1] = data[i][1] ;
        pixels[3 * i + 2] = data[i][2] ;
    }
    GLuint texture_id ;
    glGenTextures (1, &texture_id) ;
    glBindTexture (GL_TEXTURE_1D, texture_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels) ; // load the image
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //wrapping mode
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // interpolation mode
    return texture_id ;
}

// when you click and move the mouse the brightness and contrast change
void cursorMove (GLFWwindow* window, double xpos, double ypos) {
    GLint program_id ;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program_id) ;
    int xpos_view, ypos_view ;
    glfwGetWindowSize (window, &xpos_view, &ypos_view) ;
    keepRatio (&xpos_view, &ypos_view) ;
    // give the height and width of the window where we should be working on to keep the window ratio
    int viewport_size_x = xpos_view/2 ;
    int viewport_size_y = ypos_view/2 ;
    // the size of the viewport (x : width, y : height)
    /* Index of g_p_brightness and contras :   0     1
                                               2     3 */
    int num_view = getBlock (window, xpos, ypos) ;
    // num on which viewport you clicked
    posBlock (num_view, &xpos_view, &ypos_view) ;
    // return the bottom left corner of the current viewport
    if ((num_view >= 0) && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)) {
        g_p_brightness [num_view] = ((ypos_view -((float) ypos)) - viewport_size_y/2) /(viewport_size_y/2) ;
        g_p_contrast [num_view] = (((float) xpos) - xpos_view)/(viewport_size_x/2) ;
    }
}

void mouseClick (GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos ;
    glfwGetCursorPos(window, &xpos, &ypos) ;
    int num_view = getBlock (window, xpos, ypos) ;
    if ((num_view >= 0) && (button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS)) {
        // when you right click show the picture w/o changes
        g_p_brightness [num_view] = 0 ;
        g_p_contrast [num_view] = 1 ;
    }


}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
/* Index of g_p_brightness and contras :   0     1
                                           2     3 */
    if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && (action == GLFW_PRESS)) {
        double xpos, ypos ;
        glfwGetCursorPos(window, &xpos, &ypos) ;
        int num_view = getBlock (window, xpos, ypos) ;
        for (int i = 0 ; i < 4 ; ++i ) {
            g_p_brightness [i] = g_p_brightness [num_view] ;
            g_p_contrast   [i] = g_p_contrast [num_view] ;
        }
    }
}


void posBlock (int num_view, int* width, int* height) {
// take the width and height of the current window and return the coordinate of the bottom left corner.
    /* Index of g_p_brightness and contras :   0     1
                                               2     3 */
    switch (num_view) {
    case 0 :
        *width = 0 ;
        *height = *height/2 ;
        break ;
    case 1 :
        *width = *width/2 ;
        *height = *height/2 ;
        break ;
    case 2 :
        *width = 0 ;
        break ;
    case 3 :
        *width = *width/2 ;
        break ;
    }
}


void keepRatio (int* p_width, int* p_height) {
// take the width and height of the window and compute the right width and height to keep the ratio
    float ratio_window = ((float) (*p_width))/(*p_height) ;
    if (ratio_window > RATIO_INIT) {
    // too wide
        *p_width = RATIO_INIT * (*p_height) ;
    } else if (ratio_window < RATIO_INIT) {
        *p_height = *p_width / RATIO_INIT  ;
    }
}

GLuint loadShader (char* vertex_shader, char* fragment_shader) {
// load the shader and quit the program if there is a pb
// exit should be kept even with exception because LoadShaders doesn't throw exceptions
    GLuint program_id = LoadShaders(vertex_shader, fragment_shader) ;
    if (program_id == 0 ) {
            cout << "Error loading shader" << endl ;
            exit(EXIT_FAILURE) ;
    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return program_id ;
}



GLuint loadAndBindVAO () {
    GLuint vao_id ;
    //create VAO which contains every information about the location and state of the VBO in VRAM
    glGenVertexArrays(1,&vao_id) ;
    glBindVertexArray (vao_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return vao_id ;
}

GLuint* loadAndFillVBO () {
// fill the vertex buffer object with the vertex and texture coordinate
    float vertex []= { -1, -1, 0,
                       -1,  1, 0,
                        1, -1, 0,
                       -1,  1, 0,
                        1, -1, 0,
                        1,  1, 0 } ;
    float texture_coord [] = {
                             0.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f
    } ;
    GLuint *vbo_id =  new GLuint[2];
    // create VBO which allocate space in the VRAM
    glGenBuffers (2, vbo_id) ;
    // unlock it
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id[0]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id[1]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(texture_coord), texture_coord, GL_STREAM_DRAW) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return vbo_id ;
}

void generateUniformVariable (GLuint program_id, GLuint* texture_sampler, GLuint* lut_sampler, GLuint* mat_proj) {
    *texture_sampler = glGetUniformLocation (program_id, "myTextureSampler") ;
    *lut_sampler     = glGetUniformLocation (program_id, "myLutSampler") ;
    *mat_proj        = glGetUniformLocation (program_id, "projection") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void setGLFWCallbackFunction (GLFWwindow* window) {
    GLFWcursorposfun callback_cursor = &cursorMove ;
    glfwSetCursorPosCallback(window, callback_cursor) ;
    GLFWmousebuttonfun callback_mouse = &mouseClick ;
    glfwSetMouseButtonCallback(window, callback_mouse) ;
    GLFWkeyfun key_fun = &key_callback ;
    glfwSetKeyCallback(window, key_fun) ;
    /*GLFWwindowsizefun size_func = &windowSize ;
    glfwSetWindowSizeCallback(window, size_func) ;*/
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void loadSampler (GLuint texture_id, GLuint texture_sampler, GLuint lut_id, GLuint lut_sampler) {
// load the Uniform variable lut sampler and texture sampler
    glActiveTexture(GL_TEXTURE0) ;
    glBindTexture(GL_TEXTURE_2D, texture_id) ;
    glUniform1i (texture_sampler, 0) ;
    glActiveTexture(GL_TEXTURE1) ;
    glBindTexture(GL_TEXTURE_1D, lut_id) ;
    glUniform1i (lut_sampler, 0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}


void loadDataToShader (GLuint* vbo_id) {
    glEnableVertexAttribArray (0) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id[0]) ;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    // send the vertices to the vertex shader
    glEnableVertexAttribArray (1) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id[1]) ;
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void disableData () {
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void deleteMemory (GLuint program_id, GLuint vao_id, GLuint* vbo_id, GLuint texture_id, GLuint lut_id) {
    glDeleteVertexArrays (1, &vao_id) ;
    glDeleteBuffers (2, vbo_id) ;
    glDeleteProgram (program_id) ;
    glDeleteTextures(1, &texture_id) ;
    glDeleteTextures(1, &lut_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void render (GLuint texture_id, GLuint texture_sampler, GLuint* vbo_id, GLuint lut_id, GLuint lut_sampler, GLuint mat_proj) {
    loadSampler (texture_id, texture_sampler, lut_id, lut_sampler) ;
    mat4 projection = ortho(-1.0f,1.0f,-1.0f,1.0f) ;
    glUniformMatrix4fv (mat_proj, 1, GL_FALSE, &projection[0][0]) ;
    // load projection matrix into the vertex shader
    loadDataToShader (vbo_id) ;
    glDrawArrays(GL_TRIANGLES, 0 , 6) ; //render the data
}

Viewport* initViewport (GLFWwindow* window) {

/* Index of p_viewport :   0     1
                           2     3 */
    Viewport upperleft_viewport   (window,            0, HEIGHT_INIT/2, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::SAGITAL_PLANE) ;
    Viewport upperright_viewport  (window, WIDTH_INIT/2, HEIGHT_INIT/2, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::CORONAL_PLANE) ;
    Viewport bottomleft_viewport  (window,            0,             0, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::TRANSVERSE_PLANE) ;
    Viewport bottomright_viewport (window, WIDTH_INIT/2,             0, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::VOLUME_RENDERING) ;
    Viewport* p_viewport = new Viewport [4] ;

    p_viewport [0] = upperleft_viewport ;
    p_viewport [1] = upperright_viewport ;
    p_viewport [2] = bottomleft_viewport ;
    p_viewport [3] = bottomright_viewport ;

    return p_viewport ;
}

Viewport* updateViewport (GLFWwindow* window, Viewport* p_viewport) {
    /* Index of p_viewport :   0     1
                               2     3 */
    // Should be window size callback ? (p_viewport needed)
    int width, height ;
    glfwGetWindowSize (window, &width, &height) ;
    keepRatio (&width, &height) ;

    p_viewport [0].setY (height/2) ; // upper left
    p_viewport [1].setX (width/2) ;  // upper right
    p_viewport [1].setY (height/2) ; // upper right
    p_viewport [3].setX (width/2) ;// bottom right

    for (int i = 0 ; i < 4 ; i++) {
        p_viewport[i].setWidth  (width/2) ;
        p_viewport[i].setHeight (height/2) ;
    }

    return p_viewport ;
}

void loadUniformBrightAndContrast (GLuint program_id, int i) {
/* Index of g_p_brightness :   0     1
                               2     3 */
    GLuint brightness = glGetUniformLocation (program_id, "bright") ;
    GLuint contrast = glGetUniformLocation(program_id, "contr") ;
    glUniform1f (brightness, g_p_brightness[i]) ;
    // brightness 0 at the middle -1 at left corner and 1 at right corner
    glUniform1f (contrast, g_p_contrast[i]) ;
    // contrast 0 at the left corner 1 at the middle and 2 at the right corner

}

int getBlock (GLFWwindow* window, int xpos, int ypos) {
/* Index of g_p_brightness :   0     1
                               2     3
    get which viewport is at xpos and ypos */
    int width, height, num_block ;
    glfwGetWindowSize(window, &width, &height) ;
    keepRatio(&width, &height) ;
// get the width and height where we are working
    if ((xpos < width) && (ypos < height)) {
        if (xpos < width/2) {
            if (ypos < height/2) num_block = 0 ;
            else num_block = 2 ;
        } else {
            if (ypos < height/2) num_block = 1 ;
            else num_block = 3 ;
        }
    } else return -1 ;
    return num_block ;
}

void initBrightnessAndContrast () {
    g_p_brightness = new float [4] ;
    g_p_contrast = new float [4] ;
    for (int i = 0 ; i < 4 ; i++) {
        g_p_brightness [i] = 0 ;
        g_p_contrast [i] = 1 ;
    }
}


int main()
{
    GLuint vao_id, *vbo_id, texture_id, lut_id, program_id ;
    try {
        GLFWwindow* window = initGlfwAndWindow() ;
        initGlew() ;
        initGL() ;
        initBrightnessAndContrast () ;

        program_id = loadShader("TextVertexShader.vertexshader", "TextFragmentShader.fragmentshader") ;
        vao_id = loadAndBindVAO();
        vbo_id =  loadAndFillVBO() ;

        lut_id = loadLUT(g_lut_texture_data) ;
        texture_id = loadTexture() ;
        //Create LUT and texture from file

        GLuint texture_sampler, lut_sampler, mat_proj ;
        generateUniformVariable(program_id, &texture_sampler, &lut_sampler, &mat_proj) ;
        //Generate uniform variable location*/

        Viewport *p_viewport = initViewport(window) ;


        do {
            glClear ( GL_COLOR_BUFFER_BIT) ; // reset setting and screen to set previously
            glUseProgram (program_id) ; // use the shader
            p_viewport = updateViewport(window, p_viewport) ;

            for (int i = 0 ; i < 4 ; i++) {
                    loadUniformBrightAndContrast(program_id, i) ;
                    p_viewport[i].useViewport() ;
                    render (texture_id, texture_sampler, vbo_id, lut_id, lut_sampler, mat_proj) ;
            }

            disableData() ;

            glfwSwapBuffers(window) ;
            glfwPollEvents() ;
        } while (closeWindow(window) == 0)  ;
    } catch (std::exception& error)
    {
        cerr << "Error: " << error.what() << endl;
        cerr << "The program will terminate" << endl;
    }
    catch (std::string& error)
    {
        cerr << "Error: " << error << endl;
        cerr << "The program will terminate" << endl;
    }
    catch (const char* error)
    {
        cerr << "Error: " << error << endl;
        cerr << "The program will terminate" << endl;
    }
    deleteMemory (program_id, vao_id, vbo_id, texture_id, lut_id) ;

}


