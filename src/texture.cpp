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

#include <Viewport.hpp>
#include <Rectangle.hpp>
#include <Pea.hpp>
#include <OpenGLException.hpp>
#include <Image.h>
#include <map>
#include <fstream>
#include <iostream>
#include <string>

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
#define WIDTH_VIEW (WIDTH_INIT/2)

#define INTERFACE_WIDTH 400
#define INTERFACE_HEIGHT 400

#define NB_LUT_TABLE 2
#define NB_OF_VIEWPORT 4
#define TOO_WIDE 1
#define TOO_HIGH 2
#define NB_OF_VIEW 3

#define NB_OF_POINTS_2D 6
#define NB_OF_POINTS_3D 36

#define ASK_LOAD 0
#define ASK_SAVE 0
#define ASK_LOAD_PEA 0
#define ASK_SAVE_PEA 1
// 0 if you don't want to save or load rectangles data

#define TEXTURE_DIM 3
// loadAndFillVBO() texture_coord var to change manually
using namespace std;

/* CONTROL :
    press Control : brightness and contrast are the same for every viewport
    hold left click : change brightness and contrast
    press R : reset brightness and contrast to no change
    press mouse wheel : draw crosses and adapt the viewport to see the same point
    scroll : advance in the 3rd dimension of the viewport
    two right click : rectangle
    P : draw a point for peas
    C : remove line and rect from the screen
    U : reset zoom
    I/O : zoom In/Out where your cursor is (center the texture on your cursor)
*/

float* g_p_brightness, *g_p_contrast ;
// array with the value of the brightness and contrast of each viewport (same index as g_p_viewport)
int g_scroll_pos [NB_OF_VIEW] ;
// the scroll position of each viewport  (same index as g_p_viewport)
GLuint g_p_lut_tables [NB_LUT_TABLE] ;
// stores every lut table id : 0 is purple, 1 is fire
// currently not used
GLuint g_current_lut_id, g_vao_id, *g_vbo_texture_id ;
// current lut as the index of the LUT table (always purple)
// vbo_texture_id is the vertex buffer object that contains the texture coordinate
GLuint* g_p_lines_vbo_id, *g_p_lines_colour_vbo ;
// g_p_lines_vbo_id is the vertex buffer object that contains the coordinate of the vertex of the lines for each viewport (4 points for each VP)
// g_p_lines_colour_vbo is the vertex buffer object that contains the colours of the lines for each viewport (4 colours in each vbo)
GLuint g_rectangle_vbo_id, g_rectangle_colour_vbo ;
// g_p_rectangle_vbo_id is the vertex buffer object that contains the vertex of the 4 lines of the rectangle (4*2 = 8points)
// g_rectangle_colour_vbo is the vbo that contains the color of the lines of the rectangle (white)
Viewport g_p_viewport [NB_OF_VIEWPORT] ;
/* Index of g_p_viewport :   0     1
                           2     3 */
float* g_p_vertex_lines [NB_OF_VIEW] ;
// contains the coordinate of the lines to be drawn (used to fill g_p_lines_vbo_id)
bool g_draw_lines = false, g_draw_rect = false ;
//false if lines/rect shouldn't be drawn true otherwise
bool g_first_rectangle_click[NB_OF_VIEWPORT] ;
// true if the first corner of the rectangle has been selected
float g_p_rectangle_xyz [3] ;
// contains the coordinate of the top-left corner of the rectangle
map<int,Rectangle> g_p_rectangles ;
//contains only the rectangle manually written (if you add one interpolated rectangles are computed again)
map<int,Rectangle> g_p_rectangles_interpolated ;
//contains all the rectangles displayed so manually written + interpolated (this ones is used to load in File
vector<Pea> g_p_peas ;
int *g_p_number_of_peas ;
//nb of coordinate in g_p_peas_vbo
vector<double> g_p_pea_data [3] ;
GLuint *g_p_pea_vbo_id ;
double g_p_zoom [3] ;
double g_zoom_center [3][2] ;
// 0 is x coordinate 1 is y coordinate for the plan of the viewport (zoom center is in texture coordinate)

void windowSize (GLFWwindow* window, int width, int height) ;
void setGLFWCallbackFunction (GLFWwindow* window) ;
int keepRatio (int* p_width, int* p_height) ;
int getBlock (GLFWwindow* window, int xpos, int ypos) ;
void posBlock (int num_view, int* width, int* height) ;
GLuint loadAndBindVAO () ;
void setInterfaceGLFWCallbackFunction(GLFWwindow* window_interface) ;
bool checkGLError(string FILE, string FUNCTION, int LINE) ;
void updateTextureCoord (int flag, double t) ;
void windowSizeCallback (GLFWwindow* window, int width, int height) ;
void loadAndFillLinesVBO () ;
void loadAndFillRectangleVBO (Rectangle rect) ;
bool checkIfKeyDefined(map<int, Rectangle> p_rect, int key) ;
void getNumberOfPeaToRender (int flag) ;
void addPeaData (int x, int y, int z, int flag) ;






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
    window = glfwCreateWindow (WIDTH_INIT, HEIGHT_INIT, "Texture Window", NULL, NULL ) ;
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
    glClearColor(1,0,0,0) ;
    glPointSize(3) ;
    glEnable( GL_POINT_SMOOTH );

}

bool checkGLError(string FILE, string FUNCTION, int LINE) {
    GLenum err = GL_NO_ERROR ;
    const char* file = FILE.c_str() ;
    const char* func = FUNCTION.c_str() ;

#ifndef NDEBUG
     err = glGetError() ;

    if (err != GL_NO_ERROR) {
        throw OpenGLException(file, func, LINE, err) ;
    }

#endif
    return (err !=  GL_NO_ERROR) ;
}

GLuint load3DTexture (Image <float,1> *img) {
    img->loadFromMHDFile("C:/Users/Simon/Documents/GitHub/Blue-square/textures/sorted_MRI_image_data-2.mhd") ;
    GLuint texture_3D_id ;
    glGenTextures (1, &texture_3D_id) ;
    glBindTexture (GL_TEXTURE_3D, texture_3D_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexImage3D (GL_PROXY_TEXTURE_3D, 0, GL_R32F, img->getWidth(), img->getHeight(), img->getNumberOfSlices(), 0, GL_RED, GL_FLOAT, img->normaliseZeroToOne().getRawDataPointer() ) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    // Check width proxy texture
    GLint check_width_gl, check_width_img ;
    glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &check_width_gl) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

   check_width_img = (int) img->getWidth() ;
    cout << "Width with GetTexParameter : " << check_width_gl << endl  << "Width with the Image class : " << check_width_img << endl ;
    if (check_width_gl != check_width_img) {
        // Not same input imag
        // Throw error
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Error proxy texture width different of image width ") ;
    }

    glTexImage3D (GL_TEXTURE_3D, 0, GL_R32F, img->getWidth(), img->getHeight(), img->getNumberOfSlices(), 0, GL_RED, GL_FLOAT, img->normaliseZeroToOne().getRawDataPointer() ) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); //wrapping mode
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // interpolation mode

    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return texture_3D_id ;
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

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels) ; // load the image

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //wrapping mode
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // interpolation mode
    return texture_id ;
}

void loadLUTS () {
// load every lut table used
// useless because we only use g_lut_texture_data
    g_p_lut_tables [1] = loadLUT (g_lut_fire_texture_data) ;
    g_p_lut_tables [0] = loadLUT (g_lut_texture_data) ;

    g_current_lut_id = g_p_lut_tables [0];
}

void cursorMove (GLFWwindow* window, double xpos, double ypos) {
// Brightness increase at the bottom of the viewport and decrease at the top
// Contrast increase on the right decrease on the left
//mouseClick and cursorMove has same block for mouse button mb try to call mouseClick in cursorMove  ?
    int i = 0, *p_xy, width_window, height_window ;
    bool viewport_found = false;
    glfwGetWindowSize (window, &width_window, &height_window) ;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        while ((!viewport_found) && (i < NB_OF_VIEWPORT)) {
            if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
                p_xy = g_p_viewport [i].worldCoordToViewportCoord (xpos, ypos, height_window) ;
                double y_center, x_center ;
                y_center = g_p_viewport [i].getHeight()/2 ;
                x_center = g_p_viewport [i].getWidth()/2 ;
                g_p_brightness [i] = (p_xy [1] - y_center) / y_center  ;
                g_p_contrast [i] = p_xy [0] / x_center;
                viewport_found = true ;
            }
            i++ ;
        }
    }
    if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)  {
        int x, y, z, *p_xy_viewport ;
        float depth_viewport ; // z, y or x depending of the plane
        while ((!viewport_found) && (i < NB_OF_VIEW)) {
            if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
                p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord (xpos, ypos, height_window) ;
                // we get the viewport coord in p_xy_viewport
                g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
                // we get the texture coordinate x, y, z from the viewport
                for (int j = 0 ; j < NB_OF_VIEW ; j++) {
                // get Lines to draw for each view
                    int x_viewport, y_viewport ;
                    g_p_viewport [j].getViewportCoordinateFromXYZ(x, y, z, &x_viewport, &y_viewport) ;
                    // we get the viewport coord with the texture coord x, y, z
                    g_p_vertex_lines[j] = g_p_viewport [j].getVertexCoordinateFromViewportCoord(x_viewport, y_viewport) ;
                    // we get the vertex line coord from the viewport coord
                    for (int k = 0 ; k < NB_OF_VIEW ; k++) {
                    // changes the texture coord to render the right slice
                        if (k != i) {
                        // only on the viewport you didn't click on
                            depth_viewport = (k == 0) ? y % ( (int) NB_OF_PIXEL_XY ) : ((k==1) ? x % ( (int) NB_OF_PIXEL_XY) : z %  (int) NB_OF_SLICES) ;
                            updateTextureCoord(k, depth_viewport ) ;
                            g_scroll_pos [k] = depth_viewport ;
                           /* make the program crash
                           if ((checkIfKeyDefined(g_p_rectangles, depth_viewport)) && (k == TRANSVERSE_PLANE)) {
                                loadAndFillRectangleVBO(g_p_rectangles [depth_viewport]) ;
                                g_draw_rect = true ;
                           }*/
                        }
                    }
                }
                g_draw_lines = true ;
                loadAndFillLinesVBO () ;
            }
            i++ ;
        }
    }
}

bool checkIfKeyDefined(map<int, Rectangle> p_rect, int key) {
// check if key is defined in the map
    map<int, Rectangle>::iterator it_map ;
    it_map = p_rect.find (key) ;
    return (it_map != p_rect.end() ) ;
}

bool checkIfHighestKey (map<int, Rectangle> p_rect, int key) {
// check if there is a higher Key
    map<int, Rectangle>::iterator it_map_up ;
    it_map_up = p_rect.upper_bound (key) ;
    return (it_map_up != p_rect.end()) ;
}

bool checkIfLowerKey (map<int, Rectangle> p_rect, int key) {
// check if there is a lower Key
    map<int, Rectangle>::iterator it_map_low ;
    it_map_low = p_rect.lower_bound (key) ;
    return (it_map_low != p_rect.begin()) ;
}
void mouseClick (GLFWwindow* window, int button, int action, int mods) {
// right click cancel brightness and contrast changes
// Mouse wheel click draw crosses and adjust the texture coord to see the same picture
    double xpos, ypos ;
    int width_window, height_window, i = 0 ;
    bool viewport_found = false ;
    glfwGetCursorPos(window, &xpos, &ypos) ;
    glfwGetWindowSize (window, &width_window, &height_window) ;
    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_RIGHT))  {
    //rectangle works only for transverse viewport
        int x, y, z, *p_xy_viewport, depth, width_rectangle, height_rectangle ;
        while ((!viewport_found) && (i < NB_OF_VIEW)) {
            if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
                     p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord (xpos, ypos, height_window) ;
                    // we get the viewport coord in p_xy_viewport
                    g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
                    if (i == TRANSVERSE_PLANE) {
                        if ((g_first_rectangle_click[i] == true) && (z == g_p_rectangle_xyz [2])) {
                        //2nd condition check if we are on the same slice (wrong if not on transverse plane)
                            width_rectangle = x - g_p_rectangle_xyz [0] ;
                            height_rectangle = y - g_p_rectangle_xyz [1] ;
                            // width and height wrong if not on transverse
                            depth = g_p_rectangle_xyz [2];
                            g_p_viewport [i].getXYZCoordinate(g_p_rectangle_xyz [0], g_p_rectangle_xyz [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
                            // create the rectangle
                            g_p_rectangles [depth] = Rectangle(width_rectangle, height_rectangle, g_p_rectangle_xyz [0], g_p_rectangle_xyz [1],
                                                               g_p_rectangle_xyz [2], TRANSVERSE_PLANE) ;
                            g_p_rectangles_interpolated [depth] = Rectangle(width_rectangle, height_rectangle, g_p_rectangle_xyz [0], g_p_rectangle_xyz [1],
                                                               g_p_rectangle_xyz [2], TRANSVERSE_PLANE) ;
                            loadAndFillRectangleVBO (g_p_rectangles [depth]) ;
                            g_first_rectangle_click [i] = false ;
                            g_draw_rect = true ;
                        } else if ((g_first_rectangle_click[i] == true) && (z != g_p_rectangle_xyz [2])) {
                            cerr << "Error in cursorMove callback : you changed slices while drawing rectangle" << endl ;
                            updateTextureCoord(TRANSVERSE_PLANE, g_p_rectangle_xyz [2]) ;
                            g_scroll_pos [i] = g_p_rectangle_xyz [2] ;
                        } else {
                        // save the coord in global variable
                            g_p_rectangle_xyz [0] = x ;
                            g_p_rectangle_xyz [1] = y ;
                            g_p_rectangle_xyz [2] = z ;
                            // to work for every viewport g_p_rectangle should be a 2 dimensional array
                            g_first_rectangle_click [i] = true ;
                        }
                    viewport_found = true ;
                }
            }
            i++ ;
        }
    } else if ((button == GLFW_MOUSE_BUTTON_MIDDLE) && (action == GLFW_PRESS))  {
        int x, y, z, *p_xy_viewport ;
        float depth_viewport ; // z, y or x depending of the plane
        while ((!viewport_found) && (i < NB_OF_VIEW)) {
            if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
                p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord (xpos, ypos, height_window) ;
                // we get the viewport coord in p_xy_viewport
                g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
                // we get the texture coordinate x, y, z from the viewport
                for (int j = 0 ; j < NB_OF_VIEW ; j++) {
                // get Lines to draw for each view
                    int x_viewport, y_viewport ;
                    g_p_viewport [j].getViewportCoordinateFromXYZ(x, y, z, &x_viewport, &y_viewport) ;
                    // we get the viewport coord with the texture coord x, y, z
                    g_p_vertex_lines[j] = g_p_viewport [j].getVertexCoordinateFromViewportCoord(x_viewport, y_viewport) ;
                    // we get the vertex line coord from the viewport coord
                    for (int k = 0 ; k < NB_OF_VIEW ; k++) {
                    // changes the texture coord to render the right slice
                        if (k != i) {
                        // only on the viewport you didn't click on
                            depth_viewport = (k == 0) ? y % ( (int) NB_OF_PIXEL_XY ) : ((k==1) ? x % ( (int) NB_OF_PIXEL_XY) : z % (int) NB_OF_SLICES) ;
                            updateTextureCoord(k, depth_viewport ) ;
                            if ((checkIfKeyDefined(g_p_rectangles_interpolated, depth_viewport)) && (k == TRANSVERSE_PLANE)) {
                                loadAndFillRectangleVBO(g_p_rectangles_interpolated [depth_viewport]) ;
                                g_draw_rect = true ;
                            } else if (!checkIfKeyDefined(g_p_rectangles_interpolated, depth_viewport) && (k == TRANSVERSE_PLANE)) {
                                g_draw_rect = false ;
                            }
                            g_scroll_pos [k] = depth_viewport ;
                            // update the scroll_pos
                        }
                    }
                }
                g_draw_lines = true ;
                loadAndFillLinesVBO () ;
            }
            i++ ;
        }
    }

}


void loadAndFillRectangleVBO (Rectangle rect) {
    glBindVertexArray (g_vao_id) ;
    float colour_rectangle [] = {
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1,
    } ;
    float* vertex_rect = g_p_viewport [2].getRectangleCoordinateFromViewportCoord(rect) ;
    glBindBuffer (GL_ARRAY_BUFFER, g_rectangle_colour_vbo) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(colour_rectangle), colour_rectangle, GL_STREAM_DRAW) ;
    glBindBuffer (GL_ARRAY_BUFFER, g_rectangle_vbo_id) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(float)*NB_OF_POINTS*2, vertex_rect, GL_STREAM_DRAW) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;


}

void loadAndFillLinesVBO () {
// fill the g_p_lines_colour_vbo and the g_p_lines_vbo_id with g_p_vertex_lines
    glBindVertexArray (g_vao_id) ;
    int x_color, y_color ;
    float colour_sagital [] =  {
                                    0, 0, 1,
                                    0, 0, 1,
                                    1, 0, 0,
                                    1, 0, 0,

    } ;
    float colour_coronal [] =  {
                                    0, 0, 1,
                                    0, 0, 1,
                                    0, 1, 0,
                                    0, 1, 0,

    } ;
    float colour_transverse [] =  {
                                    0, 1, 0,
                                    0, 1, 0,
                                    1, 0, 0,
                                    1, 0, 0,

    } ;
    float colour [3][12] ;
    for (int j = 0 ; j <12 ; j++) {
            colour [0][j] = colour_sagital [j];
    }
    for (int j = 0 ; j <12 ; j++) {
            colour [1][j] = colour_coronal [j];
    }
    for (int j = 0 ; j <12 ; j++) {
            colour [2][j] = colour_transverse [j];
    }

    for (int i = 0 ; i < NB_OF_VIEW ; i++) {
        glBindBuffer (GL_ARRAY_BUFFER, g_p_lines_vbo_id [i]) ;
        glBufferData (GL_ARRAY_BUFFER, sizeof(float)*NB_OF_POINTS, g_p_vertex_lines[i], GL_STREAM_DRAW) ;
        glBindBuffer (GL_ARRAY_BUFFER, g_p_lines_colour_vbo [i]) ;
        glBufferData (GL_ARRAY_BUFFER, sizeof(colour[i]), colour[i], GL_STREAM_DRAW) ;
        checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void loadAndFillPeasVBO (int flag) {
    glBindVertexArray (g_vao_id) ;
    glBindBuffer (GL_ARRAY_BUFFER, g_p_pea_vbo_id [flag]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(double)*g_p_pea_data[flag].size(), g_p_pea_data[flag].data(), GL_STREAM_DRAW) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
// when you click on control brightness and contrast is the same for every viewport
/* Index of g_p_brightness and contras :   0     1
                                           2     3 */
    double xpos, ypos ;
    int width_window, height_window, i = 0 ;
    bool viewport_found = false ;
    glfwGetCursorPos(window, &xpos, &ypos) ;
    glfwGetWindowSize (window, &width_window, &height_window) ;
    while ((!viewport_found) && (i < NB_OF_VIEWPORT)) {
        if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
            viewport_found = true ;
        } else
            i++ ;
    }
    if (i < 4) {
        if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)) {
                    for (int j = 0 ; j < NB_OF_VIEWPORT  ; ++j ) {
                        if (j != i) {
                            g_p_brightness [j] = g_p_brightness [i] ;
                            g_p_contrast   [j] = g_p_contrast [i] ;
                        }
                    }
        }
        else if ((key == GLFW_KEY_R) && (action == GLFW_PRESS)) {
            // when you right click show the picture w/o changes
            g_p_brightness [i] = 0 ;
            g_p_contrast [i] = 1 ;
        }
        else if ((key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
            int *p_xy_viewport, x, y, z, depth_viewport ;
            p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord (xpos, ypos, height_window) ;
            // we get the viewport coord in p_xy_viewport
            g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
            Pea p (x, y, z) ;
            g_p_peas.push_back(p) ;
            for (int j = 0 ; j < NB_OF_VIEW ; j++) {
                getNumberOfPeaToRender (j) ;
                loadAndFillPeasVBO(j) ;
            }
        }
        else if ((key == GLFW_KEY_C)) {
            g_draw_lines = false ;
            g_draw_rect = false ;
        }
        else if ((key == GLFW_KEY_I) && (action == GLFW_PRESS)) {
            int * p_xy_viewport, x, y, z ;
            p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord(xpos, ypos, height_window) ;
            g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
            switch (i) {
            case SAGITAL_PLANE :
                g_zoom_center [i] [0] =  ((2 * x / NB_OF_PIXEL_XY) - 1) ;
                g_zoom_center [i] [1] = ((2 * z / NB_OF_SLICES) - 1) ;
                break ;
            case CORONAL_PLANE :
                g_zoom_center [i] [0] = ((2 * y / NB_OF_PIXEL_XY) - 1) ;
                g_zoom_center [i] [1] = ((2 * z / NB_OF_SLICES) - 1) ;
                break ;
            case TRANSVERSE_PLANE :
                g_zoom_center [i] [0] = ((2 * x / NB_OF_PIXEL_XY) - 1) ;
                g_zoom_center [i] [1] = ((2 * y / NB_OF_PIXEL_XY) - 1);
                break ;
            }
            g_p_zoom [i] += 0.2 ;
            if (g_zoom_center[i][0] - 1.0f/g_p_zoom [i] < -1.0f ) {
                g_zoom_center[i][0] = -1.0f + 1.0f/g_p_zoom [i] ;
            }
            if (g_zoom_center[i][0] + 1.0f/g_p_zoom [i] > 1.0f ) {
                g_zoom_center[i][0] = 1.0f - 1.0f/g_p_zoom [i] ;
            }
            if (- g_zoom_center[i][1] - 1.0f/g_p_zoom [i] < -1.0f ) {
                g_zoom_center[i][1] = 1.0f - 1.0f/g_p_zoom [i] ;
            }
            if (- g_zoom_center[i][1] + 1.0f/g_p_zoom [i] > 1.0f ) {
                g_zoom_center[i][1] = -1.0f + 1.0f/g_p_zoom [i] ;
            }
        }
        else if ((key == GLFW_KEY_O) && (action == GLFW_PRESS)) {
            int * p_xy_viewport, x, y, z ;
            p_xy_viewport = g_p_viewport [i].worldCoordToViewportCoord(xpos, ypos, height_window) ;
            g_p_viewport [i].getXYZCoordinate(p_xy_viewport [0], p_xy_viewport [1], g_scroll_pos [i], &x, &y, &z, g_p_zoom [i], g_zoom_center [i][0], g_zoom_center [i][1]) ;
            switch (i) {
            case SAGITAL_PLANE :
                g_zoom_center [i] [0] = x / NB_OF_PIXEL_XY ;
                g_zoom_center [i] [1] = z / NB_OF_SLICES ;
                break ;
            case CORONAL_PLANE :
                g_zoom_center [i] [0] = y / NB_OF_PIXEL_XY ;
                g_zoom_center [i] [1] = z / NB_OF_SLICES;
                break ;
            case TRANSVERSE_PLANE :
                g_zoom_center [i] [0] = x / NB_OF_PIXEL_XY ;
                g_zoom_center [i] [1] = y / NB_OF_PIXEL_XY ;
                break ;
            }
            if (g_p_zoom [i] >= 1.2)
                g_p_zoom [i] -= 0.2 ;
            if (g_zoom_center[i][0] - 1.0f/g_p_zoom [i] < -1.0f ) {
                g_zoom_center[i][0] = -1.0f + 1.0f/g_p_zoom [i] ;
            }
            if (g_zoom_center[i][0] + 1.0f/g_p_zoom [i] > 1.0f ) {
                g_zoom_center[i][0] = 1.0f - 1.0f/g_p_zoom [i] ;
            }
            if (- g_zoom_center[i][1] - 1.0f/g_p_zoom [i] < -1.0f ) {
                g_zoom_center[i][1] = 1.0f - 1.0f/g_p_zoom [i] ;
            }
            if (- g_zoom_center[i][1] + 1.0f/g_p_zoom [i] > 1.0f ) {
                g_zoom_center[i][1] = -1.0f + 1.0f/g_p_zoom [i] ;
            }
        }
        if (key == GLFW_KEY_U) {
            g_p_zoom [i] = 1 ;
            g_zoom_center [i][0] = 0 ;
            g_zoom_center [i][1] = 0 ;
        }

    }
}




GLuint loadShader (char* vertex_shader, char* fragment_shader) {
// load the shader and quit the program if there is a problem
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
// return the vertex buffer object with the vertex in 2D and 3D
// init the global array of 4 vbo which contains the texture coordinate of the 4 view
    glBindVertexArray (g_vao_id) ;
    float vertex []= { -3, -3, 0,
                       -3,  3, 0,
                        3, -3, 0,
                       -3,  3, 0,
                        3, -3, 0,
                        3,  3, 0
                        } ;
    float vertex_volumne_rendering []= {
                                        -1.0f, -1.0f,  1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                        -1.0f,  1.0f,  1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                        -1.0f,  1.0f,  1.0f,
                                        -1.0f,  1.0f, -1.0f,

                                        1.0f, -1.0f,  1.0f,
                                        1.0f, -1.0f, -1.0f,
                                        1.0f,  1.0f,  1.0f,
                                        1.0f, -1.0f, -1.0f,
                                        1.0f,  1.0f,  1.0f,
                                        1.0f,  1.0f  -1.0f,

                                        -1.0f,  1.0f, -1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                         1.0f,  1.0f, -1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                         1.0f,  1.0f, -1.0f,
                                         1.0f, -1.0f, -1.0f,

                                        -1.0f,  1.0f,  1.0f,
                                        -1.0f, -1.0f,  1.0f,
                                         1.0f,  1.0f,  1.0f,
                                        -1.0f, -1.0f,  1.0f,
                                         1.0f,  1.0f,  1.0f,
                                         1.0f, -1.0f,  1.0f,

                                        -1.0f, -1.0f,  1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                         1.0f, -1.0f,  1.0f,
                                        -1.0f, -1.0f, -1.0f,
                                         1.0f, -1.0f,  1.0f,
                                         1.0f, -1.0f, -1.0f,

                                        -1.0f, 1.0f,  1.0f,
                                        -1.0f, 1.0f, -1.0f,
                                         1.0f, 1.0f,  1.0f,
                                        -1.0f, 1.0f, -1.0f,
                                         1.0f, 1.0f,  1.0f,
                                         1.0f, 1.0f, -1.0f,


        } ;
    g_vbo_texture_id =  new GLuint[4];
    GLuint* vbo_id_vertex = new GLuint[2] ;
    // create VBO which allocate space in the VRAM
    glGenBuffers (4, g_vbo_texture_id) ;
    glGenBuffers (2, vbo_id_vertex) ;

    glBindBuffer (GL_ARRAY_BUFFER, vbo_id_vertex [0]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STREAM_DRAW) ;

    glBindBuffer (GL_ARRAY_BUFFER, vbo_id_vertex [1]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(vertex_volumne_rendering), vertex_volumne_rendering, GL_STREAM_DRAW) ;
    for (int i = 0 ; i < NB_OF_VIEWPORT ; i++) {
            updateTextureCoord (i, 0) ;
    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    return vbo_id_vertex ;
}

void interpolateRectangle (int slice) {
    map<int, Rectangle>::iterator it_map_low, it_map_up ;
    it_map_low = g_p_rectangles.lower_bound (slice) ;
    it_map_up = g_p_rectangles.upper_bound (slice) ;
    it_map_low-- ;
    // then we do the mean of the rectangle value in low and up and add it
    g_p_rectangles_interpolated [slice] = it_map_low->second.mean (it_map_up->second, slice) ;


}
void scrollCallback (GLFWwindow* window, double xoffset, double yoffset) {
/*scroll : advance in the 3rd dimension of the viewport : change the texture coord */
    double xpos, ypos ;
    int width_window, height_window, i = 0 ;
    bool viewport_found = false ;
    glfwGetCursorPos(window, &xpos, &ypos) ;
    glfwGetWindowSize (window, &width_window, &height_window) ;
    while ((!viewport_found) && (i < NB_OF_VIEWPORT)) {
        if (g_p_viewport [i].coordInViewport (xpos, ypos, height_window)) {
            for (int j = 0 ; j < NB_OF_VIEW ; ++j ) {
                g_scroll_pos [j] = yoffset + g_scroll_pos [j] ;
                updateTextureCoord(j, g_scroll_pos [j]) ;
                if (j == TRANSVERSE_PLANE ) {
                    int slice_number = g_scroll_pos [j] % (int) NB_OF_SLICES ;
                    if (checkIfKeyDefined(g_p_rectangles, slice_number)) {
                        loadAndFillRectangleVBO(g_p_rectangles [slice_number]) ;
                        g_draw_rect = true ;
                    } else if (checkIfHighestKey(g_p_rectangles,  slice_number ) && checkIfLowerKey (g_p_rectangles, slice_number)) {
                        interpolateRectangle(slice_number) ;
                        loadAndFillRectangleVBO(g_p_rectangles_interpolated [slice_number]) ;
                        g_draw_rect = true ;
                    } else {
                        g_draw_rect = false ;
                    }
                }
            }
            viewport_found = true ;
        }
        i++ ;
    }

}

void updateTextureCoord (int flag, double t) {
// update the Texture Coord with the t value in the 3rd dimension (t is between 0 and the max texture coord in this dimension)
// flag = 0  : moving according to y
// flag = 1 : moving according to z
// flag = 2 : moving according to z
    glBindVertexArray (g_vao_id) ;
    float* texture_coord = new float [6*TEXTURE_DIM] ;
// pb with 1.0f + t when t > 0 ? The GPU is doing the 1 modulo
    if (flag == TRANSVERSE_PLANE) {
            t = t/NB_OF_SLICES ;
            float temp_texture_coord [] = {
                                        0.0f, 3.0f, 0.0f + t,
                                        0.0f, 0.0f, 0.0f + t,
                                        3.0f, 3.0f, 0.0f + t,
                                        0.0f, 0.0f, 0.0f + t ,

                                        3.0f, 3.0f, 0.0f + t,
                                        3.0f, 0.0f, 0.0f + t,

        } ;
        for (int i = 0 ; i < 6*TEXTURE_DIM ; i++) {
            texture_coord [i] = temp_texture_coord [i] ;
        }
    } else if (flag == SAGITAL_PLANE) {
        t = t/NB_OF_PIXEL_XY ;
       float temp_texture_coord [] = {
                                        0.0f, 0.0f + t, 3.0f,
                                        0.0f, 0.0f + t, 0.0f,
                                        3.0f, 0.0f + t, 3.0f,
                                        0.0f, 0.0f + t, 0.0f,
                                        3.0f, 0.0f + t, 3.0f,
                                        3.0f, 0.0f + t, 0.0f,

        } ;

        for (int i = 0 ; i < 6*TEXTURE_DIM ; i++) {
            texture_coord [i] = temp_texture_coord [i] ;
        }
    } else if (flag == CORONAL_PLANE) {
        t = t/NB_OF_PIXEL_XY ;
        float temp_texture_coord [] = {
                                        0.0f + t, 0.0f, 3.0f,
                                        0.0f + t, 0.0f, 0.0f,
                                        0.0f + t, 3.0f, 3.0f,
                                        0.0f + t, 0.0f, 0.0f,
                                        0.0f + t, 3.0f, 3.0f,
                                        0.0f + t, 3.0f, 0.0f,

        } ;
        for (int i = 0 ; i < 6*TEXTURE_DIM ; i++) {
            texture_coord [i] = temp_texture_coord [i] ;
        }
    } else if (flag == VOLUME_RENDERING) {
        float temp_texture_coord [] = {
                                        0.0f, 0.0f, 1.0f,
                                        0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 1.0f,
                                        0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 1.0f,
                                        0.0f, 1.0f, 0.0f,

                                        1.0f, 0.0f, 1.0f,
                                        1.0f, 0.0f, 0.0f,
                                        1.0f, 1.0f, 1.0f,
                                        1.0f, 0.0f, 0.0f,
                                        1.0f, 1.0f, 1.0f,
                                        1.0f, 1.0f, 0.0f,

                                        0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f,
                                        1.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f,
                                        1.0f, 1.0f, 0.0f,
                                        1.0f, 0.0f, 0.0f,

                                        0.0f, 1.0f, 1.0f,
                                        0.0f, 0.0f, 1.0f,
                                        1.0f, 1.0f, 1.0f,
                                        0.0f, 0.0f, 1.0f,
                                        1.0f, 1.0f, 1.0f,
                                        1.0f, 0.0f, 1.0f,

                                        0.0f, 0.0f, 1.0f,
                                        0.0f, 0.0f, 0.0f,
                                        1.0f, 0.0f, 1.0f,
                                        0.0f, 0.0f, 0.0f,
                                        1.0f, 0.0f, 1.0f,
                                        1.0f, 0.0f, 0.0f,

                                        0.0f, 1.0f, 1.0f,
                                        0.0f, 1.0f, 0.0f,
                                        1.0f, 1.0f, 1.0f,
                                        0.0f, 1.0f, 0.0f,
                                        1.0f, 1.0f, 1.0f,
                                        1.0f, 1.0f, 0.0f,


        } ;
        for (int i = 0 ; i < 6*TEXTURE_DIM ; i++) {
            texture_coord [i] = temp_texture_coord [i] ;
        }
    } else {
        throw Exception(__FILE__, __FUNCTION__, __LINE__, "Wrong flag in update Texture Coord") ;
    }
    glBindBuffer (GL_ARRAY_BUFFER, g_vbo_texture_id [flag]) ;
    glBufferData (GL_ARRAY_BUFFER, sizeof(float)*6*TEXTURE_DIM, texture_coord, GL_STREAM_DRAW) ;
    if (flag < 3) {
        getNumberOfPeaToRender (flag) ;
        loadAndFillPeasVBO(flag) ;
    }
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void getNumberOfPeaToRender (int flag) {
    int slice ;
    g_p_number_of_peas [flag] = 0 ;
    g_p_pea_data [flag].clear () ;
    switch (flag) {
    case SAGITAL_PLANE :
        slice = g_scroll_pos [flag] % ((int) NB_OF_PIXEL_XY) ;
        for (int i = 0 ; i < g_p_peas.size() ; i++) {
            if (g_p_peas [i].getY () == slice) {
                addPeaData (g_p_peas[i].getX(), g_p_peas[i].getY (), g_p_peas[i].getZ(), flag) ;
            }
        }
        g_p_number_of_peas [flag] = g_p_pea_data [flag].size() /3 ;
        break ;
    case CORONAL_PLANE :
        slice = g_scroll_pos [flag] % ((int) NB_OF_PIXEL_XY) ;
        for (int i = 0 ; i < g_p_peas.size() ; i++) {
            if (g_p_peas [i].getX () == slice) {
                addPeaData (g_p_peas[i].getX(), g_p_peas[i].getY (), g_p_peas[i].getZ(), flag) ;
            }
        }
        g_p_number_of_peas [flag] = g_p_pea_data [flag].size()/3 ;

        break ;
    case TRANSVERSE_PLANE :
        slice = g_scroll_pos [flag] % (int) NB_OF_SLICES ;
        for (int i = 0 ; i < g_p_peas.size() ; i++) {
            if (g_p_peas [i].getZ () == slice) {
                addPeaData (g_p_peas[i].getX(), g_p_peas[i].getY (), g_p_peas[i].getZ(), flag) ;
            }
        }
        g_p_number_of_peas [flag] = g_p_pea_data [flag].size()/3 ;

        break ;
    }
}

void addPeaData (int x, int y, int z, int flag) {
    int x_viewport, y_viewport ;
    g_p_viewport[flag].getViewportCoordinateFromXYZ (x, y, z, &x_viewport, &y_viewport) ;
    float y_vertex, x_vertex ;
    x_vertex = (2 * x_viewport / ((float) g_p_viewport[flag].getWidth())) - 1 ;
    y_vertex = (2 * y_viewport / ((float)g_p_viewport[flag].getHeight())) - 1 ;
    g_p_pea_data[flag].push_back(x_vertex) ;
    g_p_pea_data[flag].push_back(- y_vertex) ;
    g_p_pea_data[flag].push_back(0) ;
}

void generateUniformVariable (GLuint program_id, GLuint lines_program_id, GLuint pea_program_id,
                              GLuint* texture_sampler, GLuint* lut_sampler, GLuint* mat_proj,
                              GLuint* mat_proj_lines, GLuint* mat_proj_pea) {
    *texture_sampler = glGetUniformLocation (program_id, "myTextureSampler") ;
    *lut_sampler     = glGetUniformLocation (program_id, "myLutSampler") ;
    *mat_proj        = glGetUniformLocation (program_id, "projection") ;
    *mat_proj_lines  = glGetUniformLocation (lines_program_id, "projection") ;
    *mat_proj_pea    = glGetUniformLocation (pea_program_id, "projection") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void setGLFWCallbackFunction (GLFWwindow* window) {
    GLFWcursorposfun callback_cursor = &cursorMove ;
    glfwSetCursorPosCallback(window, callback_cursor) ;

    GLFWmousebuttonfun callback_mouse = &mouseClick ;
    glfwSetMouseButtonCallback(window, callback_mouse) ;

    GLFWkeyfun key_fun = &keyCallback ;
    glfwSetKeyCallback(window, key_fun) ;

    GLFWscrollfun scroll_fun = &scrollCallback ;
    glfwSetScrollCallback(window, scroll_fun) ;

    GLFWwindowsizefun size_func = &windowSizeCallback ;
    glfwSetWindowSizeCallback(window, size_func) ;

    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}


void loadSampler (GLuint texture_id, GLuint texture_sampler, GLuint lut_sampler) {
// load the Uniform variable lut sampler and texture sampler
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glActiveTexture(GL_TEXTURE0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    if (TEXTURE_DIM == 3) {
        glBindTexture(GL_TEXTURE_3D, texture_id) ;
    } else {
        glBindTexture(GL_TEXTURE_2D, texture_id) ;
    }
    glUniform1i (texture_sampler, 0) ;
    glActiveTexture(GL_TEXTURE1) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindTexture(GL_TEXTURE_1D, g_current_lut_id) ;
    glUniform1i (lut_sampler, 0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}


void loadDataToShader (GLuint* vbo_vertex_id, GLuint vbo_texture_id) {
// enable the vertex Attrib Array
    glEnableVertexAttribArray (0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindBuffer (GL_ARRAY_BUFFER, *vbo_vertex_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    // send the vertices to the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    glEnableVertexAttribArray (1) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_texture_id) ;
    // send the texture coord
    glVertexAttribPointer(1, TEXTURE_DIM, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void loadDataLinesToShader (GLuint vbo_vertex_id, GLuint vbo_colour_id) {
// enable the vertex Attrib Array for the line shader

    glEnableVertexAttribArray (0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    // send the vertices to the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    glEnableVertexAttribArray (1) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_colour_id) ;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0) ; // tells how the data should be read
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}
void disableData () {
    glDisableVertexAttribArray (0) ;
    glDisableVertexAttribArray (1) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void deleteMemory (GLuint program_id, GLuint* vbo_id, GLuint texture_id, GLuint p_lut_id [], GLuint lines_program_id) {
    glDeleteVertexArrays (1, &g_vao_id) ;
    glDeleteBuffers (2, vbo_id) ;
    glDeleteProgram (program_id) ;
    glDeleteProgram (lines_program_id) ;
    glDeleteTextures(1, &texture_id) ;
    for (int i = 0 ; i < NB_LUT_TABLE ; i++) {
            glDeleteTextures(1, p_lut_id + i) ;
    }
    glDeleteBuffers (4, g_vbo_texture_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void render (GLFWwindow* window, GLuint texture_id, GLuint texture_sampler, GLuint* vbo_id,
             GLuint vbo_texture_id, GLuint lut_sampler, GLuint mat_proj, int nb_of_points, mat4 projection) {
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    loadSampler (texture_id, texture_sampler, lut_sampler) ;
    glUniformMatrix4fv (mat_proj, 1, GL_FALSE, &projection[0][0]) ;
    // load projection matrix into the vertex shader
    loadDataToShader (vbo_id, vbo_texture_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glDrawArrays(GL_TRIANGLES, 0 , nb_of_points) ; //render the data
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;

}

void renderLines (GLuint vbo_lines_id, GLuint vbo_colour_id, GLuint mat_proj, mat4 projection) {
    loadDataLinesToShader (vbo_lines_id, vbo_colour_id) ;
    glUniformMatrix4fv (mat_proj, 1, GL_FALSE, &projection[0][0]) ;
    glDrawArrays (GL_LINES, 0, 4) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}


void renderRect(GLuint vbo_lines_id, GLuint vbo_colour_id, GLuint mat_proj, mat4 projection) {
    loadDataLinesToShader (vbo_lines_id, vbo_colour_id) ;
    glUniformMatrix4fv (mat_proj, 1, GL_FALSE, &projection[0][0]) ;
    glDrawArrays (GL_LINES, 0, 8) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void loadDataPeaToShader (GLuint vbo_vertex_id) {
// enable the vertex Attrib Array for the line shader

    glEnableVertexAttribArray (0) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glBindBuffer (GL_ARRAY_BUFFER, vbo_vertex_id) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    // send the vertices to the vertex shader
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0) ;
}
void renderPeas (GLuint vbo_peas_id, int flag, GLuint mat_proj, mat4 projection) {
    loadDataPeaToShader (vbo_peas_id) ;
    glUniformMatrix4fv (mat_proj, 1, GL_FALSE, &projection[0][0]) ;
    glDrawArrays (GL_POINTS, 0, g_p_number_of_peas [flag]) ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}
double setViewportRatio (Image<float,1>* img, View view) {
    double ratio_screen ;
    switch (view) {
    case SAGITAL_PLANE :
        ratio_screen = img->getVoxelWidth() * img->getWidth() / (img->getVoxelDepth() * img->getNumberOfSlices()) ;
        break ;
    case CORONAL_PLANE :
        ratio_screen =  img->getVoxelHeight()*img->getHeight() / (img->getVoxelDepth() * img->getNumberOfSlices()) ;
        break ;
    case TRANSVERSE_PLANE :
        ratio_screen = img->getVoxelWidth() * img->getWidth() / (img->getVoxelHeight()*img->getHeight()) ;
        break ;
    }
    return ratio_screen ;
}

void initViewport (GLFWwindow* window, Image <float,1>* img) {

/* Index of g_p_viewport :   0     1
                           2     3 */


    double coronal_ratio_screen = setViewportRatio(img, CORONAL_PLANE) ;
    double sagital_ratio_screen = setViewportRatio(img, SAGITAL_PLANE) ;
    double transverse_ratio_screen = setViewportRatio(img, TRANSVERSE_PLANE) ;

    Viewport upperleft_viewport   (window,          0, WIDTH_VIEW, WIDTH_VIEW/sagital_ratio_screen, WIDTH_VIEW, SAGITAL_PLANE) ;
    Viewport upperright_viewport  (window, WIDTH_VIEW, WIDTH_VIEW, WIDTH_VIEW/coronal_ratio_screen, WIDTH_VIEW, CORONAL_PLANE) ;
    Viewport bottomleft_viewport  (window,           0,         0,                      WIDTH_VIEW, WIDTH_VIEW, TRANSVERSE_PLANE) ;
    Viewport bottomright_viewport (window, WIDTH_VIEW,          0,                      WIDTH_VIEW, WIDTH_VIEW, VOLUME_RENDERING) ;

    g_p_viewport [0] = upperleft_viewport ;
    g_p_viewport [1] = upperright_viewport ;
    g_p_viewport [2] = bottomleft_viewport ;
    g_p_viewport [3] = bottomright_viewport ;

    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    for (int i = 0 ; i < NB_OF_VIEW ; i++) {
            g_p_viewport [i].setViewportRatio(img) ;
    }
}

void windowSizeCallback (GLFWwindow* window, int width, int height) {
    /* Index of g_p_viewport :   0     1
                               2     3 */
    double ratio_screen ;
    int width_viewport = width/2 ;
    ratio_screen = g_p_viewport [0].getRatio() ;
   if (width_viewport + width_viewport/ratio_screen > height) {
        width_viewport = height / (1 + 1/ratio_screen) ;


    }
    g_p_viewport [0].setY (width_viewport) ; // upper left
    g_p_viewport [0].setHeight (width_viewport/ratio_screen) ;
    g_p_viewport [0].setWidth (width_viewport) ;

    ratio_screen = g_p_viewport [1].getRatio() ; // upper right
    g_p_viewport [1].setX (width_viewport) ;
    g_p_viewport [1].setY (width_viewport) ;
    g_p_viewport [1].setHeight (width_viewport/ratio_screen) ;
    g_p_viewport [1].setWidth (width_viewport) ;

    ratio_screen = g_p_viewport [2].getRatio() ; // bottom left
    g_p_viewport [2].setHeight (width_viewport) ;
    g_p_viewport [2].setWidth (width_viewport) ;

    ratio_screen = g_p_viewport [3].getRatio() ;
    g_p_viewport [3].setX (width_viewport) ; // bottom right
    g_p_viewport [3].setHeight (width_viewport) ;
    g_p_viewport [3].setWidth (width_viewport) ;

    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
}

void loadUniformBrightAndContrast (GLuint program_id, int i) {
/* Index of g_p_brightness :   0     1
                               2     3 */
    GLuint brightness = glGetUniformLocation (program_id, "bright") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    GLuint contrast = glGetUniformLocation(program_id, "contr") ;
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glUniform1f (brightness, g_p_brightness[i]) ;
    // brightness 0 at the middle -1 at left corner and 1 at right corner
    checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
    glUniform1f (contrast, g_p_contrast[i]) ;
    // contrast 0 at the left corner 1 at the middle and 2 at the right corner

}

void getRectFromFile (string file_name) {
    ifstream file_stream (file_name.c_str(), ios::in) ;
    string a_rect_in_char ;
    int width, height, slice , viewport_number ;
    double x, y ;
    if (file_stream) {
        while ( file_stream >> width >> height >> x >> y >> slice) {
            g_p_rectangles [slice] = Rectangle(width, height, x, y, slice, 2) ;
            g_p_rectangles_interpolated [slice] = Rectangle(width, height, x, y, slice, 2) ;

        }
        file_stream.close () ;
    } else
        throw Exception (__FILE__, __FUNCTION__, __LINE__, "Can't open file") ;

}

void getPeaFromFile (string file_name) {
    ifstream file_stream (file_name.c_str(), ios::in) ;
    string a_rect_in_char ;
    int x, y, z ;
    if (file_stream) {
        while ( file_stream >> x >> y >> z) {
            g_p_peas.push_back (Pea(x, y, z)) ;
        }
        file_stream.close () ;
    } else
        throw Exception (__FILE__, __FUNCTION__, __LINE__, "Can't open file") ;

}

void loadPeaFromFile () {
        char save_data ;
        string file_name ;
        cout << "Do you want to load the pea data  ? y/n " << endl ;
        cin >> save_data ;
        if (save_data == 'y') {
            cout << "File name ? " << endl ;
            cin >> file_name ;
            string path_name = "C:/Users/Simon/Documents/GitHub/Blue-square/data/" + file_name ;
            getPeaFromFile(path_name) ;
        }

}
void loadRectFromFile () {
        char save_data ;
        string file_name ;
        cout << "Do you want to load the rectangle data  ? y/n " << endl ;
        cin >> save_data ;
        if (save_data == 'y') {
            cout << "File name ? " << endl ;
            cin >> file_name ;
            string path_name = "C:/Users/Simon/Documents/GitHub/Blue-square/data/" + file_name ;
            getRectFromFile(path_name) ;
        }

}

mat4 updateProjection (double zoom, int flag) {
    switch (flag) {
    case SAGITAL_PLANE :
        return  ortho(g_zoom_center[flag][0] - 1.0f/zoom, g_zoom_center[flag][0] + 1.0f/zoom,
                  - g_zoom_center[flag][1] - 1.0f/zoom, - g_zoom_center[flag][1] + 1.0f/zoom) ;
        break ;
    case CORONAL_PLANE :
        return  ortho(g_zoom_center[flag][0] - 1.0f/zoom, g_zoom_center[flag][0] + 1.0f/zoom,
                  - g_zoom_center[flag][1] - 1.0f/zoom, - g_zoom_center[flag][1] + 1.0f/zoom) ;
        break ;
    case TRANSVERSE_PLANE :
        return  ortho(g_zoom_center[flag][0] - 1.0f/zoom, g_zoom_center[flag][0] + 1.0f/zoom,
                  - g_zoom_center[flag][1] - 1.0f/zoom, - g_zoom_center[flag][1] + 1.0f/zoom) ;
        break ;
    default :
        return ortho (-1.0f, 1.0f, -1.0f, 1.0f) ;
        break ;
    }

}

void initGlobalVariable() {
    g_p_brightness = new float [NB_OF_VIEWPORT] ;
    g_p_contrast = new float [NB_OF_VIEWPORT] ;
    for (int i = 0 ; i < NB_OF_VIEWPORT ; i++) {
        g_p_brightness [i] = 0 ;
        g_p_contrast [i] = 1 ;
        g_first_rectangle_click [i] = false ;

    }
    for (int i = 0 ; i < NB_OF_VIEW ; i++) {
        g_scroll_pos [i] = 0 ;
    }
    g_vao_id = loadAndBindVAO();
    g_p_lines_vbo_id = new GLuint [3] ;
    glGenBuffers(3, g_p_lines_vbo_id) ;
    for (int i = 0 ; i < NB_OF_VIEW ; i++ ){
        g_p_vertex_lines [i] = new float [NB_OF_POINTS] ;
    }
    g_p_lines_colour_vbo = new GLuint [3] ;
    glGenBuffers(3, g_p_lines_colour_vbo) ;
    glGenBuffers(1, &g_rectangle_colour_vbo) ;
    glGenBuffers(1, &g_rectangle_vbo_id) ;

    if (ASK_LOAD)
        loadRectFromFile() ;

    if (ASK_LOAD_PEA)
        loadPeaFromFile() ;

    g_p_number_of_peas = new int [3] ;
    for (int i = 0 ; i < NB_OF_VIEW ; i++) {
        g_p_number_of_peas [i] = 0 ;
    }
    g_p_pea_vbo_id = new GLuint [NB_OF_VIEW] ;
    glGenBuffers (3, g_p_pea_vbo_id) ;

    for (int i = 0 ; i < NB_OF_VIEW ; i++) {
        g_p_zoom [i] = 1 ;
        g_zoom_center [i][0] = 0.0 ;
        g_zoom_center [i][1] = 0.0 ;
    }


}

void saveRectInFile (string file_name) {

    map<int, Rectangle>::iterator it_map ;
    ofstream empty_file_if_exists (file_name.c_str (), ios::out | ios::trunc) ;
    empty_file_if_exists.close() ;

    for (it_map = g_p_rectangles_interpolated.begin() ; it_map != g_p_rectangles_interpolated.end() ; it_map++) {
        it_map->second.saveInFile (file_name) ;
    }
}


void writePeaInFile (string file_name) {
    ofstream file_stream (file_name.c_str(), ios::out) ;
    if (file_stream) {
        for (int i = 0 ; i < g_p_peas.size() ; i++) {
            file_stream << g_p_peas[i].getX() << " " << g_p_peas[i].getY() << " " << g_p_peas[i].getZ() << endl  ;
        }
        file_stream.close () ;
    } else
        throw Exception (__FILE__, __FUNCTION__, __LINE__, "Can't open file") ;

}

void savePeaInFile () {
    char save_data ;
    string file_name ;
    cout << "Do you want to save the pea data  ? y/n " << endl ;
    cin >> save_data ;
    if (save_data == 'y') {
        cout << "File name ? " << endl ;
        cin >> file_name ;
        string path_name = "C:/Users/Simon/Documents/GitHub/Blue-square/data/" + file_name ;
        writePeaInFile(path_name) ;
    }

}



void saveInFile () {
        char save_data ;
        string file_name ;
        cout << "Do you want to save the rectangle data  ? y/n " << endl ;
        cin >> save_data ;
        if (save_data == 'y') {
            cout << "File name ? " << endl ;
            cin >> file_name ;
            string path_name = "C:/Users/Simon/Documents/GitHub/Blue-square/data/" + file_name ;
            saveRectInFile(path_name) ;
        }

}


int main()
{
    GLuint *vbo_id, texture_id, program_id, lines_program_id, pea_program_id ;
    Image <float, 1> *img =  new Image <float,1> ;

    try {
        GLFWwindow* window = initGlfwAndWindow() ;
        initGlew() ;
        initGL() ;
        initGlobalVariable() ;

        program_id = loadShader("TextVertexShader.vertexshader", "TextFragmentShader.fragmentshader") ;
        lines_program_id = loadShader("TextureLine.vertexshader", "TextureLine.fragmentshader") ;
        pea_program_id = loadShader("Pea.vertexshader", "Pea.fragmentshader") ;
        vbo_id = loadAndFillVBO() ;
        loadLUTS() ;
        texture_id = (TEXTURE_DIM == 3) ? load3DTexture(img) : loadTexture() ;
        //Create LUT and texture from file

        GLuint texture_sampler, lut_sampler, mat_proj, mat_proj_lines, mat_proj_pea ;
        generateUniformVariable(program_id, lines_program_id, pea_program_id, &texture_sampler, &lut_sampler, &mat_proj, &mat_proj_lines, &mat_proj_pea) ;
        //Generate uniform variable location*/

        initViewport(window, img) ;
        mat4 projection = ortho(-1.0f,1.0f,-1.0f,1.0f) ;


        do {
            glClear ( GL_COLOR_BUFFER_BIT) ; // reset setting and screen to set previously
            glUseProgram (program_id) ; // use the shader
            checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
            for (int i = 0 ; i < NB_OF_VIEW ; i++) {
                projection = updateProjection (g_p_zoom [i], i) ;
                loadUniformBrightAndContrast(program_id, i) ;
                checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
                g_p_viewport[i].useViewport() ;
                checkGLError(__FILE__, __FUNCTION__, __LINE__) ;
                render (window, texture_id, texture_sampler, vbo_id, g_vbo_texture_id [i], lut_sampler, mat_proj, NB_OF_POINTS_2D, projection) ;
                disableData() ;
            }
           glUseProgram (lines_program_id) ;
           if (g_draw_lines) {
                for (int i = 0 ; i  < NB_OF_VIEW ; i++) {
                    projection = updateProjection (g_p_zoom [i], i) ;
                    g_p_viewport[i].useViewport() ;
                    renderLines (g_p_lines_vbo_id [i], g_p_lines_colour_vbo [i], mat_proj_lines, projection) ;
                    disableData() ;
                }
            }
            disableData() ;
            if (g_draw_rect) {
                projection = updateProjection (g_p_zoom [2], 2) ;
                g_p_viewport [2].useViewport() ;
                renderRect (g_rectangle_vbo_id, g_rectangle_colour_vbo, mat_proj_lines, projection) ;
                disableData() ;
            }
            glUseProgram (pea_program_id) ;
            for (int i = 0 ; i < NB_OF_VIEW ; i++) {
                projection = updateProjection (g_p_zoom [i], i) ;
                g_p_viewport [i].useViewport() ;
                renderPeas (g_p_pea_vbo_id [i], i, mat_proj_pea, projection) ;
            }

            glDisableVertexAttribArray (0) ;
            glfwSwapBuffers(window) ;
            glfwPollEvents() ;
        } while ((closeWindow(window) == 0))  ;
        if (ASK_SAVE)
            saveInFile() ;
        if (ASK_SAVE_PEA)
            savePeaInFile() ;
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
    deleteMemory (program_id, vbo_id, texture_id, g_p_lut_tables, lines_program_id) ;

}


