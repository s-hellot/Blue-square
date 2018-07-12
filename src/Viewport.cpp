#include "Viewport.hpp"


Viewport::Viewport (GLFWwindow* w, int x, int y, int height, int width, View v) :
    m_x(x), m_y (y), m_height (height), m_width (width)
{
    m_view = v ;
    m_window_parent = w ;
}

int Viewport::getX () const {
    return m_x ;
}

int Viewport::getY () const {
    return m_y ;
}

int Viewport::getHeight () const {
    return m_height ;
}

int Viewport::getWidth () const {
    return m_width ;
}

Viewport::View Viewport::getView () const {
    return m_view ;
}

GLFWwindow* Viewport::getWindow () const {
    return m_window_parent ;
}

mat4 Viewport::getCamera () const {
    return m_camera ;
}

void Viewport::setX (int x) {
    m_x = x ;
}

void Viewport::setY (int y) {
    m_y = y ;
}

void Viewport::setWidth (int width) {
    m_width = width ;
}

void Viewport::setHeight (int height) {
    m_height = height ;
}
void Viewport::setUpCamera () {
    switch (m_view) {
    case SAGITAL_PLANE :
    // SAGITAL PLANE : separates left and right part of the body (right and left for hand) x = 0
        m_camera = lookAt (vec3 (-10, 0, 0), vec3(0, 0, 0), vec3 (0, 0, 1)) ;
        break ;
    case CORONAL_PLANE :
    // CORONAL PLANE : separates ventral and dorsal part z = 0
        m_camera = lookAt (vec3 (0.0000001, 0, -10), vec3(0, 0, 0), vec3 (0, 0, 1)) ;
        break ;
    case TRANSVERSE_PLANE :
    // TRANSVERSE PLANE : separates upper and lower part y = 0
        m_camera = lookAt (vec3 (0, -10, 0), vec3(0, 0, 0), vec3 (0, 0, 1)) ;
        break ;
    case VOLUME_RENDERING :
        m_camera = lookAt (vec3 (0, -10, -10), vec3(0, 0, 0), vec3 (0, 0, 1)) ;
        break ;
    case UNKNOWN :
        std::cerr << "View unknown " << std::endl ;
        break ;
    }
}

Viewport::~Viewport () {
}

void Viewport::useViewport () {
    glViewport (m_x, m_y, m_width, m_height) ;
}

Viewport::Viewport(const Viewport& aViewport) {
    m_x = aViewport.getX() ;
    m_y = aViewport.getY() ;
    m_height = aViewport.getHeight() ;
    m_width = aViewport.getWidth() ;
    m_camera = aViewport.getCamera() ;
    m_view = aViewport.getView() ;
    m_window_parent = aViewport.getWindow() ;

}

void Viewport::operator=(const Viewport& aViewport) {
    m_x = aViewport.getX() ;
    m_y = aViewport.getY() ;
    m_height = aViewport.getHeight() ;
    m_width = aViewport.getWidth() ;
    m_camera = aViewport.getCamera() ;
    m_view = aViewport.getView() ;
    m_window_parent = aViewport.getWindow() ;
}
int* Viewport::worldCoordToViewportCoord (int x, int y) {
    int* p_xy = new int [2] ;
    p_xy[0] = x - m_x ;
    p_xy[1] = y - m_y ;
    return p_xy ;
}

int* Viewport::viewportCoordToWorldCoord (int x, int y) {
    int* p_xy = new int [2] ;
    p_xy[0] = x + m_x ;
    p_xy[1] = y + m_y ;
    return p_xy ;
}


