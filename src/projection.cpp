#include <iostream>
#include <cstdlib>
#include <stdio.h>
// GLEW to include before gl.h and glfw3.h
#include <GL/glew.h>

#include <GL/gl.h>
// GLFW : handle window and keyboard
#include <GLFW/glfw3.h>

#include <shader.hpp>
#include <Viewport.hpp>
#include <OpenGLException.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "cloud.h"
using namespace glm ;

#define WIDTH_INIT 800.0
#define HEIGHT_INIT 600.0
#define RATIO_INIT (WIDTH_INIT/HEIGHT_INIT)
using namespace std;


float g_delta_time = 0.0f ;
bool* g_first_mouse_click = new bool [4] ;
// true if it isn't the first time the mouse appears on the screen, array because first click on each viewport
float *g_last_x = new float [4], *g_last_y = new float [4] ;
// last x and last y on each viewport, following :
/* Index of p_viewport :   0     1
                           2     3 */
float *g_pitch = new float [4], *g_yaw = new float [4] ;
// yaw is init at -90 to remove the jump in x at first mouse input (still exist in z-axis)
float g_fov = 45 ;

Viewport* g_p_viewport = new Viewport [4] ;

void setGLFWCallbackFunction (GLFWwindow* window) ;


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

void initGL () {
    glClearColor(0,255,0,0) ;
    glEnable (GL_DEPTH_TEST) ;
}

GLFWwindow* initGlfwAndWindow () {
    if ( !glfwInit ()) {
        cerr << "Failed to initialize GLFW\n" << endl ;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    GLFWwindow* window ;
    window = glfwCreateWindow (WIDTH_INIT, HEIGHT_INIT, "First Window", NULL, NULL ) ;
    if (window == NULL) {
        cerr << "Failed to open GLFW window\n" << endl ;
        glfwTerminate() ;
    }
    glfwMakeContextCurrent(window) ; // current on the calling thread

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE) ;
    setGLFWCallbackFunction (window) ;

    return window ;
}

void initGlew () {
    glewExperimental = true ;
    if (glewInit () != GLEW_OK) {
        cerr << "Failed to initialize GLEW\n" << endl ;
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

    GLuint texture_id ;
    glGenTextures (1, &texture_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindTexture (GL_TEXTURE_2D, texture_id) ;
    float pixels [] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
    } ;
    glTexImage2D(GL_PROXY_TEXTURE_2D,0,GL_RED, g_cloud_texture_width, g_cloud_texture_height, 0, GL_RED, GL_BYTE, p_data) ; // load the image
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED, g_cloud_texture_width, g_cloud_texture_height, 0, GL_RED, GL_BYTE, p_data) ; // load the image

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // interpolation mode

    delete [] p_data;
    return texture_id ;
}

/*void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods){
    float camera_speed = 15.0f * g_delta_time ;
    switch (key) {
    case GLFW_KEY_W :
        g_camera_pos += camera_speed * g_camera_front ;
        break ;
    case GLFW_KEY_S :
        g_camera_pos -= camera_speed * g_camera_front ;
        break ;
    case GLFW_KEY_A :
        g_camera_pos -= normalize(cross (g_camera_front, g_camera_up))* camera_speed ;
        break ;
    case GLFW_KEY_D :
        g_camera_pos += normalize(cross (g_camera_front, g_camera_up))* camera_speed ;
        break ;
    }
}*/

int getBlock (GLFWwindow* window, int xpos, int ypos) {
/* Index of g_p_brightness :   0     1
                               2     3
    get which viewport is at xpos and ypos */
    int width, height, num_block ;
    glfwGetWindowSize(window, &width, &height) ;
    //keepRatio(&width, &height) ;
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

void mouseCallback (GLFWwindow* window, double xpos, double ypos) {
    float xoffset, yoffset ;
    int num_view = getBlock (window, xpos, ypos) ;
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && (num_view != -1)) {
        if (g_first_mouse_click[num_view]) {
            g_last_x [num_view] = xpos ;
            g_last_y [num_view] = ypos ;
            g_first_mouse_click [num_view] = false ;
        } else {
            xoffset = xpos - g_last_x [num_view] ;
            yoffset = g_last_y [num_view] - ypos ;
            // reversed with Y because y axis is bottom to top on the screen coordinate
            // but we want to look up when the mouse cursor goes up (camera y-axis is opposite)
            g_last_x [num_view] = xpos ;
            g_last_y [num_view] = ypos ;
            float sensitivity = 0.15f ;
            xoffset *= sensitivity ;
            yoffset *= sensitivity ;

            g_yaw[num_view] += yoffset ;
            g_pitch[num_view] += xoffset ;

            /*if (g_pitch[num_view] > 89) {
               g_pitch[num_view] = 89 ;
            } else if (g_pitch[num_view] < - 89) {
                g_pitch[num_view] = -89 ;
            }*/
            // you can only see above and below you not behind you
            vec3 front_direction ;
            front_direction.x = cos (radians(g_pitch[num_view])) * cos(radians(g_yaw[num_view])) ;
            front_direction.y = sin(radians(g_pitch[num_view])) ;
            front_direction.z = cos(radians(g_pitch[num_view])) * sin(radians(g_yaw[num_view])) ;
            g_p_viewport[num_view].setUpCamera() ;
            vec3 camera_pos = g_p_viewport[num_view].getCameraPosition () ;
            g_p_viewport[num_view].setCamera(lookAt(camera_pos, camera_pos + normalize(front_direction), vec3(0,0,1))) ;

        }
    }
}

void scrollCallback (GLFWwindow* window, double xoffset, double yoffset) {
    g_fov += yoffset ;
    g_fov = (g_fov > 45)? 45 : g_fov ;
    g_fov = (g_fov <  1)? 1  : g_fov ;
}

GLuint loadShader (char* vertex_shader, char* fragment_shader) {
    GLuint program_id = LoadShaders(vertex_shader, fragment_shader) ;
    if (program_id == 0 ) {
            cerr << "Error loading shader" << endl ;
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

GLuint loadAndFillVBO () {
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
    GLuint vbo_id ;
    glGenBuffers (1, &vbo_id) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW) ;
    return vbo_id ;
}

void generateUniformVariable (GLuint program_id, GLuint* texture_sampler, GLuint* proj_id, GLuint* view_id, GLuint* model_id) {
    *proj_id = glGetUniformLocation  (program_id, "projection") ;
    *view_id = glGetUniformLocation  (program_id, "view") ;
    *model_id = glGetUniformLocation  (program_id, "model") ;
    *texture_sampler = glGetUniformLocation (program_id, "textSampler") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void mouseButtonCallback (GLFWwindow* window, int button, int action, int mods) {
    if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE)) {
       for (int i = 0 ; i < 4 ; i++) {
            g_first_mouse_click[i] = true ;
       }
    }
}

void setGLFWCallbackFunction (GLFWwindow* window) {
    /*GLFWkeyfun key_func = &keyboard ;
    glfwSetKeyCallback(window, key_func) ;*/
    GLFWcursorposfun cursor_func = mouseCallback ;
    glfwSetCursorPosCallback(window, cursor_func) ;
    GLFWscrollfun scroll_func = scrollCallback ;
    glfwSetScrollCallback(window, scroll_func) ;
    GLFWmousebuttonfun mb_func = mouseButtonCallback ;
    glfwSetMouseButtonCallback(window, mb_func) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void loadSampler (GLuint texture_id, GLuint texture_sampler) {
        glActiveTexture(GL_TEXTURE0) ;
        glBindTexture(GL_TEXTURE_2D, texture_id) ;
        glUniform1i (texture_sampler, 0) ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}


void loadDataToShader (GLuint vbo_id) {
    glBindBuffer (GL_ARRAY_BUFFER, vbo_id) ;
    glEnableVertexAttribArray(0) ;
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) 0 ) ;
    glEnableVertexAttribArray (1) ;
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (3*sizeof(float))) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void disableData () {
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void deleteMemory (GLuint program_id, GLuint vao_id, GLuint* vbo_id, GLuint texture_id) {
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1) ;
    glDeleteVertexArrays (1, &vao_id) ;
    glDeleteBuffers (1, vbo_id) ;
    glDeleteTextures(1, &texture_id) ;
    glDeleteProgram (program_id) ;
    delete [] g_p_viewport ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void loadUniformMatrix (GLuint proj_id, GLuint view_id, Viewport current_viewport) {
    mat4 projection, view ;
    //view = lookAt(g_camera_pos, g_camera_pos + g_camera_front, g_camera_up) ;
    projection = perspective(radians (g_fov), 1024.0f/768.0f, 0.10f, 100.0f) ;
    view = current_viewport.getCamera() ;
    glUniformMatrix4fv (proj_id, 1, GL_FALSE, &projection[0][0]) ;
    glUniformMatrix4fv (view_id, 1, GL_FALSE, &view[0][0]) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void drawCubes (vec3* cube_positions, GLuint model_id) {
    for (int i = 0 ; i < 10 ; i++) {
            mat4 model = translate(mat4(1.0f),cube_positions[i]) ;
            //float angle = glfwGetTime()*20 ;
            //model = rotate(model, angle, vec3(1,1,1)) ;
            glUniformMatrix4fv (model_id, 1, GL_FALSE, &model[0][0]) ;
            glDrawArrays(GL_TRIANGLES, 0 , 12*3) ; //render the data
    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void render (GLuint program_id, GLuint proj_id, GLuint view_id, GLuint texture_id, GLuint texture_sampler, GLuint model_id, vec3 cube_positions [], Viewport current_viewport ) {
        glUseProgram (program_id) ; // use the shader

        loadUniformMatrix(proj_id, view_id, current_viewport) ;

        loadSampler(texture_id, texture_sampler) ;

        drawCubes (cube_positions, model_id) ;

}

Viewport* initViewport (GLFWwindow* window) {
    Viewport upperleft_viewport (window,             0, HEIGHT_INIT/2, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::SAGITAL_PLANE) ;
    Viewport upperright_viewport (window, WIDTH_INIT/2, HEIGHT_INIT/2, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::CORONAL_PLANE) ;
    Viewport bottomleft_viewport (window,            0,             0, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::TRANSVERSE_PLANE) ;
    Viewport bottomright_viewport (window, WIDTH_INIT/2,            0, HEIGHT_INIT/2, WIDTH_INIT/2, Viewport::VOLUME_RENDERING) ;

    g_p_viewport [0] = upperleft_viewport ;
    g_p_viewport [1] = upperright_viewport ;
    g_p_viewport [2] = bottomleft_viewport ;
    g_p_viewport [3] = bottomright_viewport ;

    return g_p_viewport ;

}

void initGlobalVariable (GLFWwindow* window) {
        g_p_viewport = initViewport(window) ;

        for (int i = 0 ; i < 4 ; i++) {
            g_first_mouse_click [i] = true ;
        }
        g_yaw [0] = 0 ;
        g_pitch [0] = 0 ;
        g_yaw [1] = 90 ;
        g_pitch [1] = 0 ;
        g_yaw [2] = 0 ;
        g_pitch [2] = 90 ;
        g_yaw [3] = 45 ;
        g_pitch [3] = 45 ;
}
int main()
{
    GLuint program_id, vao_id, vbo_id, texture_id ;
    try {
        GLFWwindow* window = initGlfwAndWindow() ;
        initGlew() ;
        initGL () ;
        initGlobalVariable (window) ;

        program_id = loadShader("ProjecVertexShader.vertexshader", "ProjecFragmentShader.fragmentshader") ;
        vec3 cube_positions[] = {
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

        GLuint proj_id, view_id, model_id, texture_sampler ;
        generateUniformVariable (program_id, &texture_sampler, &proj_id, &view_id, &model_id) ;

        vao_id  = loadAndBindVAO();
        vbo_id = loadAndFillVBO() ;

        loadDataToShader(vbo_id) ;

        texture_id = loadTexture() ;

        float last_frame = 0.0f ;

        do {
            float current_frame = glfwGetTime() ;
            g_delta_time = current_frame - last_frame ;
            last_frame = current_frame ;
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ; // reset setting and screen to set previously
            checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
            for (int i = 0 ; i < 4 ; i++) {
                g_p_viewport[i].useViewport() ;
                render (program_id, proj_id, view_id, texture_id, texture_sampler, model_id, cube_positions, g_p_viewport[i]) ;
            }
            glfwSwapBuffers(window) ;
            glfwPollEvents() ; // process events already in the event queue
            //getKey uses qwerty keyboard
        } while (closeWindow(window) == 0)  ;
        // no finally block, the id variable are known only in try block
        disableData() ;
    }
    catch (std::exception& error)
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

     deleteMemory(program_id, vao_id, &vbo_id, texture_id) ;

    return 0;
}
