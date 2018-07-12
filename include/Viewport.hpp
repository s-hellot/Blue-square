#ifndef DEF_VIEWPORT
#define DEF_VIEWPORT

#include <iostream>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm ;


class Viewport {

public :
     typedef enum View { UNKNOWN, SAGITAL_PLANE, CORONAL_PLANE, TRANSVERSE_PLANE, VOLUME_RENDERING } ;
// SAGITAL PLANE : separates left and right part of the body (right and left for hand) x = 0
// CORONAL PLANE : separates ventral and dorsal part z = 0
// TRANSVERSE PLANE : separates upper and lower part y = 0
    Viewport (GLFWwindow* w = 0, int x = 0, int y = 0, int height = 0, int width = 0, View v = UNKNOWN) ;
    Viewport(const Viewport& aViewport) ;
    ~Viewport () ;

    void operator=(const Viewport& aViewport) ;

    int getX () const ;
    int getY() const ;
    int getHeight () const ;
    int getWidth () const ;
    View getView () const ;
    GLFWwindow* getWindow () const ;
    mat4 getCamera ()  const;
    vec3 getCameraPosition () const ;

    void setX (int x) ;
    void setY (int y) ;
    void setHeight (int height) ;
    void setWidth (int width) ;
    void setCamera (mat4 camera) ;

    void useViewport () const ;
    void setUpCamera() ;

    int* worldCoordToViewportCoord (int x, int y) ;
    // return an array index 0 x,index 1 y in viewport coordinate
    int* viewportCoordToWorldCoord (int x, int y) ;
    // return an array index 0 x,index 1 y in world coordinate

private :
    int m_x ;
    int m_y ; // position of the bottom left corner of the viewport
    int m_height ;
    int m_width ; // height and width of the viewport
    View m_view ;
    GLFWwindow* m_window_parent ;
    mat4 m_camera ;
    vec3 m_camera_position ;
    // matrix camera, model, view

};
#endif // DEF_VIEWPORT
