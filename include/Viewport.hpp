#ifndef DEF_VIEWPORT
#define DEF_VIEWPORT

#include <iostream>
#include <GLFW/glfw3.h>
#include <Image.h>
#include <Rectangle.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define RED_AXIS 1
#define GREEN_AXIS 2
#define BLUE_AXIS 3
// x is red, y is green, z is blue
#define NB_OF_POINTS 12
#define NB_OF_SLICES 98.0
#define NB_OF_PIXEL_XY 320.0
using namespace glm ;

typedef enum View {  SAGITAL_PLANE = 0, CORONAL_PLANE = 1, TRANSVERSE_PLANE = 2, VOLUME_RENDERING = 3, UNKNOWN = 4 } ;
// SAGITAL PLANE : separates left and right part of the body (right and left for hand) x = 0
// CORONAL PLANE : separates ventral and dorsal part z = 0
// TRANSVERSE PLANE : separates upper and lower part y = 0
class Viewport {

public :

    Viewport (GLFWwindow* w = 0, int x = 0, int y = 0, int height = 0, int width = 0, View v = UNKNOWN) ;
    Viewport(const Viewport& aViewport) ;
    ~Viewport () ;

    void operator=(const Viewport& aViewport) ;

    int getX () const ;
    int getY() const ;
    int getHeight () const ;
    int getWidth () const ;
    View getView () const ;
    double getRatio () const ;
    GLFWwindow* getWindow () const ;
    mat4 getCamera ()  const;
    vec3 getCameraPosition () const ;
    int getDepthCoordinate (int x, int y, int z) const  ;

    void setX (int x) ;
    void setY (int y) ;
    void addX (int x) ;
    void addY (int y) ;
    void setHeight (int height) ;
    void setWidth (int width) ;
    void setCamera (mat4 camera) ;
    void setViewportRatio (Image<float, 1>* img) ;

    void useViewport () const ;
    void setUpCamera() ;

    void getXYZCoordinate (int x_viewport, int y_viewport, double slice, int* x, int* y, int* z, double zoom, double zoom_center_x, double zoom_center_y) ;
    void getViewportCoordinateFromXYZ (int x, int y, int z, int* x_viewport, int* y_viewport) ;
    float* getVertexCoordinateFromViewportCoord (int x, int y) ;
    float* getRectangleCoordinateFromViewportCoord (Rectangle rect) ;



    int* worldCoordToViewportCoord (int x, int y, int height_window) ;
    // return an array index 0 x,index 1 y in viewport coordinate
    int* viewportCoordToWorldCoord (int x, int y) ;
    // return an array index 0 x,index 1 y in world coordinate might not work

    bool coordInViewport (int x, int y, int height_window) ;

private :
    int m_x ;
    int m_y ; // position of the bottom left corner of the viewport
    int m_height ;
    int m_width ; // height and width of the viewport
    double m_ratio_screen ;
    int m_image_width ;
    int m_image_height ;
    int m_image_depth ;
    View m_view ;
    GLFWwindow* m_window_parent ;
    mat4 m_camera ;
    vec3 m_camera_position ;
    // matrix camera, model, view

};
#endif // DEF_VIEWPORT
